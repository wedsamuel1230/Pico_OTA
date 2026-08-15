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
 *   Pico W:   "2MB (Sketch: 1MB, FS: 1MB)"
 *   Pico 2 W: "4MB (Sketch: 2MB, FS: 2MB)"
 *   ⚠️  DO NOT select a "No FS" option - OTA needs filesystem space!
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
 * 2. After successful upload, UNCOMMENT LED variables at top of sketch:
 *    - const int ledPin = LED_BUILTIN;
 *    - unsigned long lastBlink = 0;
 *    - const unsigned long blinkIntervalMs = 500;
 * 3. UNCOMMENT LED blink logic in loop() on Core 0
 * 4. Upload again via OTA (wireless) - you'll see the LED start blinking!
 * 5. This proves:
 *    ✓ OTA works (code updated wirelessly)
 *    ✓ Core 0 main code runs (LED blinks)
 *    ✓ Core 1 OTA runs independently in background
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * CONFIGURATION:
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * Edit Wi-Fi credentials and OTA settings in secret.h:
 * • ssid = "Your_SSID" - Your Wi-Fi network name
 * • password = "Your_PASSWORD" - Your Wi-Fi password
 * • hostname = "pico-ota" - Device name shown in Arduino IDE Network Ports
 * • otaPassword = "admin" - Optional OTA upload password
 * 
 * For more details, see README.md
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/
#include <Arduino.h>
#include "secret.h"

void setup() {
  Serial.begin(115200);
  delay(1000); // Wait for serial connection
  
  Serial.println();
  Serial.println("[MAIN] Raspberry Pi Pico W Dual-Core OTA Example");
  Serial.println("[MAIN] Core 0: Application code (this message)");
  Serial.println("[MAIN] Core 1: OTA server (running independently)");

  
  // Uncomment these lines AFTER first USB upload to enable LED blink on Core 0
  // pinMode(ledPin, OUTPUT);
  // digitalWrite(ledPin, LOW);
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
  
 

