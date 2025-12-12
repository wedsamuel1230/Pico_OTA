/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * Raspberry Pi Pico W - Dual-Core OTA Example
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * WHAT THIS DOES:
 * • Core 0: Runs your main application code (with optional LED blink)
 * • Core 1: Handles OTA exclusively - prevents blocking your main app
 * • OTA runs continuously in background on Core 1 without interfering with Core 0
 * 
 * BENEFITS OVER SINGLE-CORE:
 * ✓ Main application on Core 0 never blocked by OTA operations
 * ✓ Responsive real-time tasks on Core 0 (sensors, motors, timers, etc.)
 * ✓ OTA runs independently on Core 1 with dedicated resources
 * ✓ Better performance and responsiveness overall
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * ARDUINO IDE SETUP (MUST DO BEFORE UPLOADING):
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * STEP 1: Select Your Board
 *   Tools → Board → Raspberry Pi RP2040 Boards → "Raspberry Pi Pico W"
 *   (or "Raspberry Pi Pico 2 W" if you have that model)
 * 
 * STEP 2: Configure Flash Size ⚠️ CRITICAL!
 *   Tools → Flash Size → "2MB (Sketch: 1MB, FS: 1MB)"
 *   ⚠️  DO NOT select "2MB (No FS)" - OTA needs filesystem space!
 * 
 * STEP 3: First Upload (USB)
 *   • Connect Pico W via USB cable
 *   • Tools → Port → Select your port:
 *     - Windows: COMx (e.g., COM3)
 *     - Mac/Linux: /dev/ttyACM0 or /dev/cu.usbmodem*
 *   • Click Upload button
 *   • Open Serial Monitor (115200 baud) and note the IP address
 * 
 * STEP 4: Wireless OTA Uploads (After First Upload)
 *   • Tools → Port → Network Ports → Select your device (shows hostname & IP)
 *   • Click Upload - it uploads wirelessly! 🎉
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * TESTING OTA WITH LED BLINK:
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * 1. Upload this sketch via USB first (LED code is commented out)
 * 2. After successful upload, UNCOMMENT all LED-related lines below:
 *    - Line 66-68: LED variables
 *    - Line 75-77: pinMode and digitalWrite in setup()
 *    - Line 85-91: LED blink logic in loop() on Core 0
 * 3. Upload again via OTA (wireless) - you'll see the LED start blinking!
 * 4. This proves:
 *    ✓ OTA works (code updated wirelessly)
 *    ✓ Core 0 main code runs (LED blinks)
 *    ✓ Core 1 OTA runs independently in background
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * CONFIGURATION:
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * • Edit ssid and password below with your Wi-Fi credentials
 * • Optional: Change hostname and otaPassword for security
 * • Pro tip: Use secret.h file for credentials (keeps them out of git)
 * 
 * For more details, see README.md
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/
#include <Arduino.h>
#include <pico_ota.h>

//#include "secret.h" //uncomment this line to include Wi-Fi credentials from secret.h

// If you use secret.h, define the following variables in that file:
// const char *ssid = WIFI_SSID;
// const char *password = WIFI_PASSWORD;

const char *ssid = "Your_SSID"; //change your Wi-Fi name here
const char *password = "Your_PASSWORD"; //change your Wi-Fi name here
const char *hostname = "pico-ota"; // optional: set a custom hostname
const char *otaPassword = "admin";  // optional: set an OTA password

// ============================================================================
// CORE 0 APPLICATION CODE (Main application with optional LED blink)
// ============================================================================

// LED blink indicator (COMMENTED by default - uncomment after first USB upload)
// const int ledPin = LED_BUILTIN;
// unsigned long lastBlink = 0;
// const unsigned long blinkIntervalMs = 500;

void setup() {
  Serial.begin(115200);
  delay(1000); // Wait for serial connection
  
  Serial.println();
  Serial.println("[MAIN] Raspberry Pi Pico W Dual-Core OTA Example");
  Serial.println("[MAIN] Core 0: Application code (this message)");
  Serial.println("[MAIN] Core 1: OTA server (running independently)");
  Serial.println();
  
  // Uncomment these lines AFTER first USB upload to enable LED blink on Core 0
  // pinMode(ledPin, OUTPUT);
  // digitalWrite(ledPin, LOW);
}

void setup1() {
  // Core 1 setup - runs once when Core 1 starts
  Serial.println("[OTA] Core 1 OTA server initializing...");
  otaSetup(ssid, password, hostname, otaPassword);
  Serial.println("[OTA] Core 1 OTA ready - waiting for uploads");
}
void loop() {
  // Core 0 main application loop - runs independently from Core 1 OTA server
  // This code never blocks, allowing responsive handling of sensors, timers, etc.
  // OPTIONAL: Uncomment to test OTA with LED blink (after first USB upload):
  // const unsigned long now = millis();
  // if (now - lastBlink >= blinkIntervalMs) {
  //   lastBlink = now;
  //   digitalWrite(ledPin, !digitalRead(ledPin));
  // }
  
  // Add your main application code here:
  // - Read sensors
  // - Control motors/actuators
  // - Process data
  // All of this runs WITHOUT BLOCKING for OTA updates!
  // The OTA server on Core 1 handles uploads independently.
  
  delay(100); // Adjust as needed for your application
}

void loop1() {
  // Core 1 OTA server loop - runs continuously on Core 1
  // Handles all OTA requests independently from Core 0
  otaLoop();
}
  
 


