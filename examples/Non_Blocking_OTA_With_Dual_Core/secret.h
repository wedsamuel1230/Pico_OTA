/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * Wi-Fi Credentials & Core 1 OTA Server Configuration
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * This file contains:
 * 1. Wi-Fi credentials (EDIT THESE!)
 * 2. Core 1 setup1() and loop1() functions for OTA server
 * 3. Production-ready OTA configuration (callbacks, timeout, safety features)
 * 
 * ⚠️  SECURITY: Add this file to .gitignore to keep credentials private!
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/

#include <Arduino.h>
#include <pico_ota.h>

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Wi-Fi & OTA Configuration - EDIT THESE VALUES!
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
const char *ssid = "Your_SSID";           // ← Your Wi-Fi network name
const char *password = "Your_PASSWORD";   // ← Your Wi-Fi password
const char *hostname = "pico-ota-dual";   // ← Device hostname (shows in network ports)
const char *otaPassword = "admin";        // ← OTA upload password (change for security!)

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Global State (shared between cores via extern in main sketch)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
bool otaReady = false;      // Core 1 sets this when OTA is ready
int blinkPattern = 0;       // Core 1 updates this based on connection status

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// OTA Callback Functions (executed on Core 1)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

// Called when OTA update begins
void onOtaStart() {
  Serial.println("\n[OTA Core 1] Update Started!");
  Serial.println("[OTA Core 1] Do not power off or reset the device.");
  // Note: LED control happens on Core 0, signaled via blinkPattern
}

// Called periodically during OTA upload with progress
void onOtaProgress(unsigned int current, unsigned int total) {
  static unsigned int lastPercent = 0;
  unsigned int percent = (current * 100) / total;
  
  // Print every 10% to avoid flooding serial
  if (percent >= lastPercent + 10) {
    Serial.printf("[OTA Core 1] Progress: %u%% (%u / %u bytes)\n", percent, current, total);
    lastPercent = percent;
  }
}

// Called when OTA update completes successfully
void onOtaEnd() {
  Serial.println("\n[OTA Core 1] Update Complete!");
  Serial.println("[OTA Core 1] Device will restart now...");
}

// Called if OTA update encounters an error
void onOtaError(int error) {
  Serial.printf("\n[OTA Core 1] Error: %d\n", error);
  Serial.println("[OTA Core 1] Update failed. Device continues normal operation.");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Core 1 Setup - Runs ONCE when Core 1 initializes
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void setup1() {
  // Small delay to let Core 0 initialize serial first
  delay(500);
  
  Serial.println("\n[OTA Core 1] Initializing OTA server...");
  Serial.println("[OTA Core 1] Production-ready configuration enabled:");
  Serial.println("[OTA Core 1]   • Non-blocking WiFi (15s timeout)");
  Serial.println("[OTA Core 1]   • Filesystem safety (no auto-format)");
  Serial.println("[OTA Core 1]   • Progress callbacks enabled");
  
  // Set blink pattern to indicate connecting state (Core 0 will read this)
  blinkPattern = 1;  // Slow blink = connecting
  
  // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  // Configure Production-Ready OTA Settings
  // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  
  // Set WiFi connection timeout (prevents infinite blocking)
  otaSetWifiTimeout(15000);  // 15 seconds
  
  // Disable filesystem auto-format (production safety - prevents accidental data loss)
  otaSetFsAutoFormat(false);
  
  // Register callback functions for user feedback
  otaOnStart(onOtaStart);
  otaOnProgress(onOtaProgress);
  otaOnEnd(onOtaEnd);
  otaOnError(onOtaError);
  
  // Enable WiFi auto-reconnect (v1.4.0 feature)
  otaSetAutoReconnect(true);
  otaSetReconnectInterval(30000);  // Retry every 30 seconds
  
  // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  // Attempt OTA Initialization with Timeout
  // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  
  Serial.println("[OTA Core 1] Connecting to WiFi...");
  
  bool success = otaSetupWithTimeout(
    ssid, 
    password, 
    15000,        // 15 second WiFi timeout
    hostname, 
    otaPassword,
    false         // Don't auto-format filesystem (production safety)
  );
  
  if (success) {
    otaReady = true;
    blinkPattern = 2;  // Solid LED = ready
    Serial.println("[OTA Core 1] ✓ OTA server is ready for wireless uploads!");
    Serial.println("[OTA Core 1] ✓ Core 0 application continues independently");
  } else {
    otaReady = false;
    blinkPattern = 3;  // Fast blink = failed
    Serial.println("[OTA Core 1] ✗ OTA setup failed (WiFi timeout or FS mount issue)");
    Serial.println("[OTA Core 1] ✗ Check WiFi credentials in secret.h");
    Serial.println("[OTA Core 1] ✗ Core 0 continues in offline mode");
  }
  
  Serial.println("[OTA Core 1] Setup complete.\n");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Core 1 Loop - Runs CONTINUOUSLY on Core 1
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void loop1() {
  // Handle OTA requests if successfully initialized
  if (otaReady && otaIsReady()) {
    otaLoop();  // Process OTA updates, auto-reconnect, web server, etc.
  }
  
  // Print status every 30 seconds (optional monitoring)
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 30000) {
    lastStatus = millis();
    Serial.printf("[OTA Core 1] Status: WiFi %s, OTA %s\n",
                  otaIsConnected() ? "Connected" : "Disconnected",
                  otaIsReady() ? "Ready" : "Not Ready");
  }
  
  // Small delay to be nice to other tasks
  delay(10);
}