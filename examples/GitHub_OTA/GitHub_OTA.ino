/**
 * @file GitHub_OTA.ino
 * @brief GitHub Release Auto-Update Example for Pico W and ESP32
 * 
 * This example demonstrates automatic firmware updates from GitHub Releases.
 * The device checks for new releases and updates itself when a new version
 * is available.
 * 
 * Compatible with:
 * - Raspberry Pi Pico W / Pico 2 W
 * - ESP32 boards
 * 
 * Setup:
 * 1. Create a GitHub repository for your project
 * 2. Create a Release with a .bin firmware file attached
 * 3. Configure the repo details below
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
