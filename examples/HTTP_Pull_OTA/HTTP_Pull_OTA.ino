/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * HTTP Pull-Based OTA Example — Pull Firmware from Web Server
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * WHAT THIS DOES:
 * 1. Connects to Wi-Fi and checks for firmware updates periodically
 * 2. Downloads new firmware from HTTP server when available
 * 3. Automatically installs update and reboots device
 * 4. Perfect for production deployments with centralized update server
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * ARDUINO IDE SETUP (MUST DO BEFORE UPLOADING):
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * FOR PICO W:
 *   STEP 1: Select Board
 *     Tools → Board → Raspberry Pi RP2040 Boards → "Raspberry Pi Pico W"
 *   
 *   STEP 2: Configure Flash Size ⚠️ CRITICAL!
 *     Tools → Flash Size → "2MB (Sketch: 1MB, FS: 1MB)"
 *     ⚠️  DO NOT select "2MB (No FS)" - OTA needs filesystem!
 *   
 *   STEP 3: First Upload (USB Required)
 *     • Connect via USB, select port (COMx on Windows, /dev/ttyACM0 on Linux/Mac)
 *     • Click Upload, open Serial Monitor (115200 baud), note IP address
 * 
 * FOR ESP32:
 *   STEP 1: Select Board
 *     Tools → Board → ESP32 Arduino → your ESP32 model
 *   
 *   STEP 2: First Upload (USB Required)
 *     • Connect via USB, select port
 *     • May need to hold BOOT button during upload
 *     • Open Serial Monitor (115200 baud), note IP address
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * GENERATE .BIN FIRMWARE FILE (REQUIRED FOR HTTP PULL OTA):
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * 1. In Arduino IDE: Sketch → Export Compiled Binary (Ctrl+Alt+S)
 * 2. Find .bin file in sketch folder: <sketch_name>.ino.bin
 * 3. Upload .bin to your web server (Apache, Nginx, Python http.server, etc.)
 * 4. Update FIRMWARE_URL below to point to your server
 * 
 * Example server setup (Python):
 *   python -m http.server 8000
 *   Then set: FIRMWARE_URL = "http://192.168.1.100:8000/firmware.bin"
 * 
 * ⚠️  IMPORTANT:
 *   • Pico W: .bin must be compiled with SAME Flash Size settings (include FS)
 *   • ESP32: .bin must match your board type (ESP32, ESP32-S2, ESP32-C3, etc.)
 *   • Test .bin on spare device before production deployment!
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * HOW TO USE HTTP PULL OTA:
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * 1. Upload this sketch via USB (device connects to WiFi)
 * 2. Generate .bin file of updated code (Sketch → Export Compiled Binary)
 * 3. Upload .bin to your web server
 * 4. Device checks server every 5 minutes (configurable)
 * 5. When new firmware found, device downloads, installs, and reboots
 * 6. Check Serial Monitor to verify update success
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * CONFIGURATION:
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * • Edit secret.h with WiFi credentials (WIFI_SSID, WIFI_PASSWORD)
 * • Update FIRMWARE_URL below to your server address
 * • Optional: Adjust CHECK_INTERVAL_MS for update frequency
 * • Optional: Change CURRENT_VERSION with each release for tracking
 * 
 * Compatible with: Pico W, Pico 2 W, ESP32, ESP32-S2, ESP32-C3
 * For more details, see README.md
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/

#include <pico_ota.h>
#include <WiFi.h>
#include "secret.h"  // Contains WIFI_SSID and WIFI_PASSWORD

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Configuration
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

// Firmware URL - change this to your server
const char* FIRMWARE_URL = "http://your-server.com/firmware.bin";

// Current firmware version (for version checking)
const char* CURRENT_VERSION = "1.0.0";

// Check for updates every 5 minutes
const unsigned long CHECK_INTERVAL_MS = 5 * 60 * 1000;

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Global State
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

unsigned long lastCheckMs = 0;
bool updatePending = false;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println();
  Serial.println("╔═══════════════════════════════════════════╗");
  Serial.println("║    HTTP Pull-Based OTA Example            ║");
  Serial.println("╚═══════════════════════════════════════════╝");
  Serial.println();
  
  // Set up OTA callbacks
  otaOnStart([]() {
    Serial.println("[Update] Starting firmware update...");
  });
  
  otaOnProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("[Update] Progress: %u%%\r\n", (progress * 100) / total);
  });
  
  otaOnEnd([]() {
    Serial.println("[Update] Complete! Rebooting...");
  });
  
  otaOnError([](int error) {
    Serial.printf("[Update] Error: %d\n", error);
  });
  
  // Enable auto-reconnect for reliability
  otaSetAutoReconnect(true);
  otaSetReconnectInterval(30000);  // 30 seconds
  
  // Connect to WiFi and initialize OTA
  Serial.print("[Setup] Firmware version: ");
  Serial.println(CURRENT_VERSION);
  
  if (otaSetupWithTimeout(WIFI_SSID, WIFI_PASSWORD, 30000)) {
    Serial.println("[Setup] WiFi connected and OTA ready");
    Serial.print("[Setup] IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("[Setup] Failed to connect to WiFi");
    // Device can still function offline
  }
  
  Serial.println();
  Serial.println("[Setup] Ready! Checking for updates periodically...");
}

void loop() {
  // Handle OTA events (ArduinoOTA, auto-reconnect)
  otaLoop();
  
  // Check for firmware updates periodically
  if (millis() - lastCheckMs >= CHECK_INTERVAL_MS || lastCheckMs == 0) {
    lastCheckMs = millis();
    checkForUpdate();
  }
  
  // Your application code here
  delay(10);
}

void checkForUpdate() {
  if (!otaIsConnected()) {
    Serial.println("[Update] Skipping check - WiFi not connected");
    return;
  }
  
  Serial.println();
  Serial.println("[Update] Checking for firmware update...");
  
  // Option 1: Simple update without version checking
  // The server will always send the firmware
  // int result = otaUpdateFromUrl(FIRMWARE_URL);
  
  // Option 2: Update with version checking
  // Server can compare versions using x-ESP8266-version header (or x-Pico-version)
  int result = otaUpdateFromUrl(FIRMWARE_URL, CURRENT_VERSION);
  
  // Option 3: Update from host/port/path separately
  // int result = otaUpdateFromHost("your-server.com", 80, "/firmware.bin", CURRENT_VERSION);
  
  switch (result) {
    case OTA_UPDATE_OK:
      // Device will reboot automatically
      break;
      
    case OTA_UPDATE_NO_UPDATE:
      Serial.println("[Update] Already running latest version");
      break;
      
    case OTA_UPDATE_NO_WIFI:
      Serial.println("[Update] No WiFi connection");
      break;
      
    case OTA_UPDATE_HTTP_ERROR:
      Serial.println("[Update] HTTP error - check server URL");
      break;
      
    case OTA_UPDATE_FAILED:
    default:
      Serial.println("[Update] Update failed");
      break;
  }
}
