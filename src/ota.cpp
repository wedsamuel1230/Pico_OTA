#include "ota.h"

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
#include <WiFi.h>

namespace {
bool ensureLittleFsMounted() {
  if (LittleFS.begin()) {
    Serial.println("[OTA] LittleFS mounted");
    return true;
  }

  Serial.println("[OTA] LittleFS mount failed, trying to format...");
  if (LittleFS.format() == true && LittleFS.begin() == true) {
    Serial.println("[OTA] LittleFS formatted and mounted");
    return true;
  }

  Serial.println("[OTA] ERROR: LittleFS unavailable (check Flash Size partition includes FS)");
  return false;
}

void connectWifi(const char *ssid, const char *password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("[OTA] Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println();
}
}  // namespace

void otaSetup(const char *ssid,
              const char *password,
              const char *hostname,
              const char *otaPassword) {
  connectWifi(ssid, password);
  Serial.print("[OTA] WiFi connected, IP: ");
  Serial.println(WiFi.localIP());

  if (!ensureLittleFsMounted()) {
    Serial.println("[OTA] OTA disabled because filesystem is missing");
    return;
  }

  if (hostname && *hostname) {
    ArduinoOTA.setHostname(hostname);
    Serial.print("[OTA] Hostname set to: ");
    Serial.println(hostname);
  }
  if (otaPassword && *otaPassword) {
    ArduinoOTA.setPassword(otaPassword);
    Serial.println("[OTA] OTA password enabled");
  }

  Serial.println("[OTA] Starting ArduinoOTA");
  ArduinoOTA.begin();
  Serial.println("[OTA] Ready for OTA updates");
}

void otaLoop() {
  ArduinoOTA.handle();
}
