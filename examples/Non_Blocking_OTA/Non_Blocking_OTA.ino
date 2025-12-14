/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * Non-Blocking OTA Example — Production-Ready OTA with Timeouts & Callbacks
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * WHAT THIS DEMONSTRATES:
 * 1. Non-blocking WiFi connection with configurable timeout (15s)
 * 2. OTA callbacks for real-time progress feedback during updates
 * 3. Status functions to monitor WiFi and OTA readiness
 * 4. Filesystem auto-format control (disabled for production safety)
 * 5. Graceful fallback if WiFi fails - application continues offline
 * 6. LED visual status indicators (connecting, ready, failed)
 * 7. Production-ready patterns: no infinite loops, safe defaults
 * 
 * PERFECT FOR:
 * • Production environments where blocking is unacceptable
 * • Devices that must continue operation even if OTA fails
 * • Applications requiring user feedback during updates
 * • Deployments needing filesystem data protection
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
 *     • Click Upload, open Serial Monitor (115200 baud)
 *     • Watch LED status and note IP address (if WiFi succeeds)
 * 
 * FOR ESP32:
 *   STEP 1: Select Board
 *     Tools → Board → ESP32 Arduino → your ESP32 model
 *   
 *   STEP 2: First Upload (USB Required)
 *     • Connect via USB, select port
 *     • May need to hold BOOT button during upload
 *     • Open Serial Monitor (115200 baud)
 *     • Watch LED and note IP address
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * LED STATUS PATTERNS (VISUAL FEEDBACK):
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * • OFF:              Not initialized yet
 * • SLOW BLINK (1s):  Connecting to WiFi... (timeout: 15s)
 * • SOLID ON:         OTA ready! Can accept wireless uploads
 * • FAST BLINK (250ms): OTA failed (check WiFi credentials in secret.h)
 * • SOLID ON (update): OTA update in progress (don't power off!)
 * 
 * If LED blinks fast, device continues in offline mode - fully functional!
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * TESTING NON-BLOCKING OTA:
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * TEST 1: Verify Non-Blocking Behavior
 *   1. Upload via USB with correct WiFi credentials
 *   2. Watch Serial Monitor - should connect within 15s
 *   3. LED turns solid = OTA ready
 *   4. Status messages print every 10s showing "WiFi Connected, OTA Ready"
 * 
 * TEST 2: Verify Timeout & Graceful Fallback
 *   1. Edit secret.h with WRONG WiFi credentials
 *   2. Upload via USB
 *   3. Watch LED blink slowly for ~15 seconds (timeout period)
 *   4. LED switches to fast blink = failed, offline mode
 *   5. Serial shows "OTA setup failed" but application continues!
 *   6. Status messages print every 10s showing "WiFi Disconnected, OTA Not Ready"
 * 
 * TEST 3: Verify OTA Update with Callbacks
 *   1. After TEST 1 succeeds, perform wireless upload
 *   2. Tools → Port → Network Ports → Select device
 *   3. Click Upload
 *   4. Watch Serial Monitor for progress: "[APP] OTA Progress: 10%... 20%..."
 *   5. LED stays solid during update
 *   6. Device reboots automatically when complete
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * CONFIGURATION:
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * • Edit secret.h with WiFi credentials (ssid, password, hostname, otaPassword)
 * • WiFi timeout: 15 seconds (otaSetWifiTimeout in setup())
 * • FS auto-format: DISABLED for production safety (otaSetFsAutoFormat(false))
 * • Status update interval: 10 seconds (see loop() function)
 * • LED pin: Auto-detected (LED_BUILTIN, GPIO 2 for ESP32, GPIO 25 for Pico W)
 * 
 * Production Safety Features:
 *   ✓ No infinite WiFi connection loops (15s timeout)
 *   ✓ Filesystem won't auto-format if mount fails (protects user data)
 *   ✓ Application continues if OTA setup fails (offline mode)
 *   ✓ Real-time progress feedback via callbacks and LED
 *   ✓ Status monitoring functions (otaIsConnected(), otaIsReady())
 * 
 * Compatible with: Pico W, Pico 2 W, ESP32, ESP32-S2, ESP32-C3
 * For more details, see README.md
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/

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
