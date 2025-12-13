// ESP32_OTA_test — Minimal example using PICO_OTA on ESP32
// Upload once via USB, then use Arduino IDE Network Port to upload wirelessly.

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
