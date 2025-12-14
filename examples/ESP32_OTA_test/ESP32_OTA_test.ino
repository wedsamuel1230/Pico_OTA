/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * ESP32 - OTA (Over-The-Air) Update Example
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * WHAT THIS DOES:
 * 1. Connects to Wi-Fi and enables OTA via initial USB upload
 * 2. After that, you can upload wirelessly (no USB cable needed!)
 * 3. Uses LED blink to test OTA updates visually
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * ARDUINO IDE SETUP (ESP32):
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * STEP 1: Install ESP32 Board Support
 *   File → Preferences → Additional Board Manager URLs → Add:
 *   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
 *   Tools → Board → Boards Manager → Search "esp32" → Install
 * 
 * STEP 2: Select Your ESP32 Board
 *   Tools → Board → ESP32 Arduino → Select your board:
 *   - "ESP32 Dev Module" (generic ESP32)
 *   - "ESP32-S2 Dev Module" (ESP32-S2)
 *   - "ESP32-C3 Dev Module" (ESP32-C3)
 *   - Or your specific board model
 *
 * STEP 3: First Upload (USB)
 *   • Connect ESP32 via USB cable
 *   • Tools → Port → Select your port:
 *     - Windows: COMx (e.g., COM3)
 *     - Mac: /dev/cu.usbserial-* or /dev/cu.SLAB_USBtoUART
 *     - Linux: /dev/ttyUSB0 or /dev/ttyACM0
 *   • Hold BOOT button (if needed) during upload
 *   • Click Upload button
 *   • Open Serial Monitor (115200 baud) and note the IP address
 * 
 * STEP 4: Wireless OTA Uploads (After First Upload)
 *   • Tools → Port → Network Ports → Select your device (shows hostname & IP)
 *   • Click Upload - it uploads wirelessly! 🎉
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * TESTING OTA UPDATES:
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * 1. Upload this sketch via USB (LED blinks at 500ms by default)
 * 2. Change the blink interval below (e.g., 200ms for faster blink)
 * 3. Upload again via OTA (wireless) - you'll see the blink rate change!
 * 4. This proves OTA is working and you can update your code wirelessly
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * CONFIGURATION:
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * • Edit secret.h with your Wi-Fi credentials (ssid, password)
 * • Optional: Change hostname and otaPassword for security
 * • LED Pin: GPIO 2 (adjust if needed)
 * 
 * For more details, see README.md
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/
#include <pico_ota.h>
#include "secret.h"  // Contains ssid, password, hostname, otaPassword

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("[Example] ESP32 OTA test starting...");

  otaSetup(ssid, password, hostname, otaPassword);

  // Configure an LED for visual feedback (GPIO 2 is common on many ESP32 dev boards)
  const int ledPin = 2;
  pinMode(ledPin, OUTPUT);
}

void loop() {
  otaLoop();

  // Optional: Blink LED to observe updates
  static unsigned long last = 0;
  const unsigned long interval = 500;
  const unsigned long now = millis();
  if (now - last >= interval) {
    last = now;
    static int ledPin = 2; // LED pin set in setup; keep consistent here
    digitalWrite(ledPin, !digitalRead(ledPin));
  }
}
