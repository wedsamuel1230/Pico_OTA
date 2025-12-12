//this template file is to be created by the user to store their Wi-Fi credentials
#include <Arduino.h>
#include <pico_ota.h>

const char *ssid = "Your_SSID"; //change your Wi-Fi name here
const char *password = "Your_PASSWORD"; //change your Wi-Fi name here
const char *hostname = "pico-ota"; // optional: set a custom hostname
const char *otaPassword = "admin";  // optional: set an OTA password

void setup1() {
  // Core 1 setup - runs once when Core 1 starts
  Serial.println("[OTA] Core 1 OTA server initializing...");
  otaSetup(ssid, password, hostname, otaPassword);
  Serial.println("[OTA] Core 1 OTA ready - waiting for uploads");
}

void loop1() {
  // Core 1 OTA server loop - runs continuously on Core 1
  // Handles all OTA requests independently from Core 0
  otaLoop();
}