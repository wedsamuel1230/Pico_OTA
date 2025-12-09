// Raspberry Pi Pico W OTA example
// Connects to Wi‑Fi (STA), enables Arduino OTA, and blinks the onboard LED.
// Edit WIFI_SSID / WIFI_PASSWORD in secret.h (or override the macros below).

#include <Arduino.h>
#include <ota.h>

const char *ssid = "Your_SSID";
const char *password = "Your_PASSWORD";
const char *hostname = "pico-ota";
const char *otaPassword = "strong-ota-pass";  // set to nullptr or "" to disable

const int ledPin = LED_BUILTIN;
unsigned long lastBlink = 0;
const unsigned long blinkIntervalMs = 500;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.println("[MAIN] Starting OTA setup");
  otaSetup(ssid, password, hostname, otaPassword);
}

void loop() {
  const unsigned long now = millis();
  if (now - lastBlink >= blinkIntervalMs) {
    lastBlink = now;
    digitalWrite(ledPin, !digitalRead(ledPin));
  }
  otaLoop();
}


