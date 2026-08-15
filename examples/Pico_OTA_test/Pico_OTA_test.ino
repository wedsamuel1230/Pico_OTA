/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * Raspberry Pi Pico W - OTA (Over-The-Air) Update Example
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * WHAT THIS DOES:
 * 1. Connects to Wi-Fi and enables OTA via initial USB upload
 * 2. After that, you can upload wirelessly (no USB cable needed!)
 * 3. Uncomment LED blink code to test OTA updates visually
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
 * TESTING OTA UPDATES:
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * 1. Upload this sketch via USB (LED code is commented out)
 * 2. Uncomment the LED lines below (variables and loop code)
 * 3. Upload again via OTA (wireless) - you'll see the LED start blinking!
 * 4. This proves OTA is working and you can update your code wirelessly
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
const char *hostname = "admin"; // optional: set a custom hostname
const char *otaPassword = "admin";  // optional: set an OTA password

// Uncomment the lines below to test OTA updates with LED blink indicator
// const int ledPin = LED_BUILTIN;
// unsigned long lastBlink = 0;
// const unsigned long blinkIntervalMs = 500;

void setup() {
  Serial.begin(115200);
  
  Serial.println("[MAIN] Starting OTA setup");
  otaSetup(ssid, password, hostname, otaPassword);
  // Uncomment these lines to enable LED blink (useful for testing OTA updates)
  // pinMode(ledPin, OUTPUT);
  // digitalWrite(ledPin, LOW);

  // Your setup code here...
}

void loop() {
  otaLoop(); // Must call this frequently to handle OTA requests
  // Uncomment these lines to enable LED blink (useful for testing OTA updates)
  // const unsigned long now = millis();
  // if (now - lastBlink >= blinkIntervalMs) {
  //   lastBlink = now;
  //   digitalWrite(ledPin, !digitalRead(ledPin));
  // }

  // Your loop code here...
}
  
 

