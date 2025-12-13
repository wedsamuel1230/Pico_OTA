/**
 * @file HTTP_Pull_OTA.ino
 * @brief HTTP Pull-Based OTA Example for Pico W and ESP32
 * 
 * This example demonstrates how to download and install firmware updates
 * from an HTTP server. The device checks for updates periodically and
 * downloads new firmware when available.
 * 
 * Compatible with:
 * - Raspberry Pi Pico W / Pico 2 W
 * - ESP32 boards
 * 
 * @author Pico_OTA Library
 * @version 1.4.0
 */

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
