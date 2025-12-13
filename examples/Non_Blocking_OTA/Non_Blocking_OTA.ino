/*
 * Non-Blocking OTA Example
 * 
 * Demonstrates:
 * - Wi-Fi connection timeout (15 seconds)
 * - OTA callbacks for progress feedback
 * - Status functions to check Wi-Fi and OTA readiness
 * - Filesystem auto-format control (disabled for production safety)
 * - Graceful fallback if Wi-Fi connection fails
 * 
 * This example shows how to use the library in production environments
 * where you need:
 * - Non-blocking behavior (no infinite loops)
 * - User feedback during OTA updates
 * - Protection against accidental FS data loss
 * - Ability to continue operation even if OTA fails to start
 */

#include <pico_ota.h>
#include "secret.h"

// LED configuration (adjust for your board)
#if defined(LED_BUILTIN)
  const int LED_PIN = LED_BUILTIN;
#elif defined(ARDUINO_ARCH_ESP32)
  const int LED_PIN = 2;  // GPIO 2 on most ESP32 boards
#else
  const int LED_PIN = 25;  // Pico W default
#endif

// State tracking
bool otaReady = false;
unsigned long lastBlink = 0;
int blinkPattern = 0;  // 0 = off, 1 = slow (connecting), 2 = solid (ready), 3 = fast (failed)

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// OTA Callbacks
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void onOtaStart() {
  Serial.println("\n[APP] OTA Update Started!");
  Serial.println("[APP] Do not power off or reset the device.");
  digitalWrite(LED_PIN, HIGH);  // Turn on LED during update
}

void onOtaProgress(unsigned int current, unsigned int total) {
  static unsigned int lastPercent = 0;
  unsigned int percent = (current * 100) / total;
  
  // Only print every 10% to avoid flooding serial
  if (percent >= lastPercent + 10) {
    Serial.printf("[APP] OTA Progress: %u%% (%u / %u bytes)\n", percent, current, total);
    lastPercent = percent;
  }
}

void onOtaEnd() {
  Serial.println("\n[APP] OTA Update Complete!");
  Serial.println("[APP] Device will restart now...");
}

void onOtaError(int error) {
  Serial.printf("\n[APP] OTA Error: %d\n", error);
  Serial.println("[APP] OTA update failed. Device will continue normal operation.");
  digitalWrite(LED_PIN, LOW);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Setup
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void setup() {
  Serial.begin(115200);
  delay(2000);  // Wait for serial monitor
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("\n\n======================================");
  Serial.println("Non-Blocking OTA Example");
  Serial.println("======================================\n");
  
  // Configure OTA behavior BEFORE calling otaSetup
  otaSetWifiTimeout(15000);      // 15 second timeout (default is 30s)
  otaSetFsAutoFormat(false);     // Disable auto-format for production safety
  
  // Register callbacks for user feedback
  otaOnStart(onOtaStart);
  otaOnProgress(onOtaProgress);
  otaOnEnd(onOtaEnd);
  otaOnError(onOtaError);
  
  // Attempt OTA setup with timeout
  blinkPattern = 1;  // Slow blink while connecting
  Serial.println("[APP] Attempting to connect to WiFi...");
  
  bool success = otaSetupWithTimeout(
    ssid, 
    password, 
    15000,          // 15 second WiFi timeout
    hostname, 
    otaPassword,
    false           // Don't auto-format FS (production safety)
  );
  
  if (success) {
    otaReady = true;
    blinkPattern = 2;  // Solid on when ready
    Serial.println("[APP] OTA is ready for updates!");
    Serial.println("[APP] Application continues running normally.");
  } else {
    otaReady = false;
    blinkPattern = 3;  // Fast blink when failed
    Serial.println("[APP] OTA setup failed (WiFi timeout or FS mount issue).");
    Serial.println("[APP] Application continues in offline mode.");
    Serial.println("[APP] Check your WiFi credentials and try again.");
  }
  
  Serial.println("\n[APP] Setup complete. Entering main loop...\n");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Main Loop
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void loop() {
  // Handle OTA updates if ready
  if (otaReady && otaIsReady()) {
    otaLoop();
  }
  
  // LED status indicator
  updateLED();
  
  // Your application code goes here
  // This continues running even if OTA failed to start
  
  // Example: Print status every 10 seconds
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 10000) {
    lastStatus = millis();
    Serial.printf("[APP] Status: WiFi %s, OTA %s\n",
                  otaIsConnected() ? "Connected" : "Disconnected",
                  otaIsReady() ? "Ready" : "Not Ready");
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// LED Helper
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void updateLED() {
  unsigned long now = millis();
  
  switch (blinkPattern) {
    case 0:  // Off
      digitalWrite(LED_PIN, LOW);
      break;
      
    case 1:  // Slow blink (connecting) - 1 second period
      if (now - lastBlink > 1000) {
        lastBlink = now;
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      }
      break;
      
    case 2:  // Solid (ready)
      digitalWrite(LED_PIN, HIGH);
      break;
      
    case 3:  // Fast blink (failed) - 250ms period
      if (now - lastBlink > 250) {
        lastBlink = now;
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      }
      break;
  }
}
