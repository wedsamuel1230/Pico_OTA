/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * GitHub Release Auto-Update OTA Example — Auto-Update from GitHub
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * WHAT THIS DOES:
 * 1. Connects to Wi-Fi and checks GitHub Releases periodically
 * 2. Compares current firmware version with latest GitHub release
 * 3. Automatically downloads and installs new release when available
 * 4. Perfect for open-source projects with distributed deployments
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
 * GENERATE .BIN FIRMWARE & SETUP GITHUB RELEASE:
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * STEP 1: Generate .bin Firmware File
 *   • In Arduino IDE: Sketch → Export Compiled Binary (Ctrl+Alt+S)
 *   • Find .bin file in sketch folder: <sketch_name>.ino.bin
 *   • Rename for clarity: firmware-pico-v1.1.0.bin or firmware-esp32-v1.1.0.bin
 * 
 * STEP 2: Create GitHub Repository (if not already done)
 *   • Create new repo on GitHub or use existing project
 *   • Push your Arduino sketch code to the repository
 * 
 * STEP 3: Create GitHub Release
 *   • Go to your repo → Releases → "Create a new release"
 *   • Tag version: v1.1.0 (use semantic versioning)
 *   • Release title: "Firmware v1.1.0" (descriptive title)
 *   • Attach your .bin file to the release (drag & drop or click to upload)
 *   • Publish release
 * 
 * STEP 4: Configure This Sketch
 *   • Update GITHUB_OWNER and GITHUB_REPO below
 *   • Update CURRENT_VERSION to match your release (e.g., "1.0.0")
 *   • Optional: Set ASSET_PATTERN to filter firmware files
 * 
 * ⚠️  IMPORTANT:
 *   • Pico W: .bin must be compiled with SAME Flash Size settings (include FS)
 *   • ESP32: .bin must match your board type (ESP32, ESP32-S2, ESP32-C3, etc.)
 *   • Use consistent naming: firmware-pico.bin or firmware-esp32.bin
 *   • Test .bin on spare device before publishing release!
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * HOW TO USE GITHUB RELEASE OTA:
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * 1. Upload this sketch via USB (device connects to WiFi)
 * 2. Device checks GitHub every hour (configurable)
 * 3. When new release detected, device automatically downloads and installs
 * 4. Device reboots with new firmware
 * 5. Check Serial Monitor to verify update
 * 
 * Workflow for releasing updates:
 *   1. Update CURRENT_VERSION in sketch to new version (e.g., "1.1.0")
 *   2. Generate .bin file (Sketch → Export Compiled Binary)
 *   3. Create GitHub Release with new version tag (e.g., v1.1.0)
 *   4. Attach .bin file to release
 *   5. All deployed devices auto-update within 1 hour
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * CONFIGURATION:
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * • Edit secret.h with WiFi credentials (WIFI_SSID, WIFI_PASSWORD)
 * • Update GITHUB_OWNER and GITHUB_REPO below (your GitHub username/repo)
 * • Update CURRENT_VERSION with each release
 * • Optional: Set ASSET_PATTERN to filter firmware files (*.bin matches all)
 * • Optional: Adjust CHECK_INTERVAL_MS for update check frequency
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

// GitHub repository settings
// Example: For https://github.com/username/my-project
const char* GITHUB_OWNER = "username";
const char* GITHUB_REPO = "my-project";

// Current firmware version - update this with each release!
const char* CURRENT_VERSION = "1.0.0";

// Optional: Firmware asset filename pattern
// Use "*" as wildcard. Leave empty to match any .bin file.
// Examples:
//   "firmware.bin"           - exact match
//   "firmware-*.bin"         - matches firmware-pico.bin, firmware-esp32.bin, etc.
//   ""                       - matches any .bin file
const char* ASSET_PATTERN = "*.bin";

// Check for updates every 1 hour
const unsigned long CHECK_INTERVAL_MS = 60 * 60 * 1000;

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Global State
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

unsigned long lastCheckMs = 0;
bool updateAvailable = false;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println();
  Serial.println("╔═══════════════════════════════════════════╗");
  Serial.println("║    GitHub Release Auto-Update Example     ║");
  Serial.println("╚═══════════════════════════════════════════╝");
  Serial.println();
  
  Serial.print("[Setup] Current version: ");
  Serial.println(CURRENT_VERSION);
  Serial.print("[Setup] GitHub repo: ");
  Serial.print(GITHUB_OWNER);
  Serial.print("/");
  Serial.println(GITHUB_REPO);
  Serial.println();
  
  // Configure GitHub OTA
  otaSetGitHubRepo(GITHUB_OWNER, GITHUB_REPO);
  otaSetCurrentVersion(CURRENT_VERSION);
  if (strlen(ASSET_PATTERN) > 0) {
    otaSetGitHubAssetName(ASSET_PATTERN);
  }
  
  // Set up OTA callbacks
  otaOnStart([]() {
    Serial.println("[OTA] Starting firmware update...");
    Serial.println("[OTA] DO NOT power off the device!");
  });
  
  otaOnProgress([](unsigned int progress, unsigned int total) {
    static int lastPercent = -1;
    int percent = (progress * 100) / total;
    if (percent != lastPercent && percent % 10 == 0) {
      Serial.printf("[OTA] Progress: %d%%\n", percent);
      lastPercent = percent;
    }
  });
  
  otaOnEnd([]() {
    Serial.println("[OTA] Update complete! Rebooting...");
  });
  
  otaOnError([](int error) {
    Serial.printf("[OTA] Error code: %d\n", error);
  });
  
  // Enable auto-reconnect
  otaSetAutoReconnect(true);
  otaSetReconnectInterval(60000);  // 1 minute
  otaSetMaxReconnectAttempts(10);
  
  // WiFi event callbacks
  otaOnWifiDisconnect([]() {
    Serial.println("[WiFi] Connection lost");
  });
  
  otaOnWifiReconnect([]() {
    Serial.print("[WiFi] Reconnected, IP: ");
    Serial.println(WiFi.localIP());
  });
  
  // Connect to WiFi
  Serial.println("[Setup] Connecting to WiFi...");
  
  if (otaSetupWithTimeout(WIFI_SSID, WIFI_PASSWORD, 30000, "github-ota-device")) {
    Serial.println("[Setup] WiFi connected!");
    Serial.print("[Setup] IP Address: ");
    Serial.println(WiFi.localIP());
    
    // Check for updates immediately on boot
    Serial.println();
    checkForUpdate();
  } else {
    Serial.println("[Setup] WiFi connection failed");
    Serial.println("[Setup] Will retry with auto-reconnect");
  }
  
  Serial.println();
  Serial.println("[Setup] Device ready!");
}

void loop() {
  // Handle OTA events
  otaLoop();
  
  // Check for updates periodically
  if (millis() - lastCheckMs >= CHECK_INTERVAL_MS) {
    lastCheckMs = millis();
    checkForUpdate();
  }
  
  // Your application code here
  delay(10);
}

void checkForUpdate() {
  if (!otaIsConnected()) {
    Serial.println("[GitHub] Skipping check - no WiFi");
    return;
  }
  
  Serial.println("[GitHub] Checking for new release...");
  
  char latestVersion[32];
  int result = otaCheckGitHubUpdate(latestVersion, sizeof(latestVersion));
  
  switch (result) {
    case OTA_UPDATE_OK:
      // New version available!
      Serial.println();
      Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
      Serial.println("         NEW FIRMWARE VERSION AVAILABLE!");
      Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
      Serial.print("  Current: ");
      Serial.println(CURRENT_VERSION);
      Serial.print("  Latest:  ");
      Serial.println(latestVersion);
      Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
      Serial.println();
      
      // Auto-update
      Serial.println("[GitHub] Starting automatic update...");
      doGitHubUpdate();
      break;
      
    case OTA_UPDATE_NO_UPDATE:
      Serial.print("[GitHub] Already running latest version (");
      Serial.print(CURRENT_VERSION);
      Serial.println(")");
      break;
      
    case OTA_UPDATE_NO_WIFI:
      Serial.println("[GitHub] No WiFi connection");
      break;
      
    case OTA_UPDATE_HTTP_ERROR:
      Serial.println("[GitHub] Failed to fetch release info");
      Serial.println("[GitHub] Check repo name and network connection");
      break;
      
    case OTA_UPDATE_PARSE_ERROR:
      Serial.println("[GitHub] Failed to parse GitHub response");
      break;
      
    case OTA_UPDATE_NO_ASSET:
      Serial.println("[GitHub] No matching firmware asset in release");
      Serial.println("[GitHub] Check asset pattern or release attachments");
      break;
      
    default:
      Serial.printf("[GitHub] Check failed with code: %d\n", result);
      break;
  }
}

void doGitHubUpdate() {
  int result = otaUpdateFromGitHub();
  
  // If we get here, update failed (success would reboot)
  switch (result) {
    case OTA_UPDATE_NO_UPDATE:
      Serial.println("[GitHub] No update needed");
      break;
    case OTA_UPDATE_FAILED:
      Serial.println("[GitHub] Firmware download/install failed");
      break;
    default:
      Serial.printf("[GitHub] Update failed with code: %d\n", result);
      break;
  }
}
