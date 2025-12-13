/**
 * @file WebBrowser_OTA.ino
 * @brief Web Browser Firmware Upload Example for Pico W and ESP32
 * 
 * This example starts a web server that allows you to upload firmware
 * updates directly from a web browser. Simply navigate to:
 *   http://<device-ip>/update
 * 
 * Compatible with:
 * - Raspberry Pi Pico W / Pico 2 W
 * - ESP32 boards
 * 
 * @author Pico_OTA Library
 * @version 1.4.0
 */

#include <pico_ota.h>
#include <WiFi.h>
#include "secret.h"  // Contains WIFI_SSID and WIFI_PASSWORD

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Configuration
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

// Web server port
const uint16_t WEB_SERVER_PORT = 80;

// Optional: Enable authentication for the update page
// Set to empty strings to disable authentication
const char* WEB_USERNAME = "";  // e.g., "admin"
const char* WEB_PASSWORD = "";  // e.g., "secret123"

// Device hostname
const char* HOSTNAME = "pico-ota-device";

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Global State
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

unsigned long lastStatusMs = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println();
  Serial.println("╔═══════════════════════════════════════════╗");
  Serial.println("║    Web Browser OTA Upload Example         ║");
  Serial.println("╚═══════════════════════════════════════════╝");
  Serial.println();
  
  // Set up optional authentication
  if (strlen(WEB_USERNAME) > 0 && strlen(WEB_PASSWORD) > 0) {
    otaSetWebCredentials(WEB_USERNAME, WEB_PASSWORD);
    Serial.println("[Setup] Web authentication enabled");
  }
  
  // Enable auto-reconnect
  otaSetAutoReconnect(true);
  otaSetReconnectInterval(30000);
  
  // Set up callbacks for WiFi events
  otaOnWifiDisconnect([]() {
    Serial.println("[WiFi] Disconnected - web server may be unavailable");
  });
  
  otaOnWifiReconnect([]() {
    Serial.println("[WiFi] Reconnected!");
    Serial.print("[WiFi] IP Address: ");
    Serial.println(WiFi.localIP());
  });
  
  // Connect to WiFi
  Serial.println("[Setup] Connecting to WiFi...");
  
  if (otaSetupWithTimeout(WIFI_SSID, WIFI_PASSWORD, 30000, HOSTNAME)) {
    Serial.println("[Setup] WiFi connected!");
    Serial.print("[Setup] IP Address: ");
    Serial.println(WiFi.localIP());
    
    // Start the web server
    otaStartWebServer(WEB_SERVER_PORT);
    
    printInstructions();
  } else {
    Serial.println("[Setup] Failed to connect to WiFi");
    Serial.println("[Setup] Please check credentials and restart");
  }
}

void loop() {
  // Handle OTA events (ArduinoOTA, auto-reconnect, web server)
  otaLoop();
  
  // Print status every 30 seconds
  if (millis() - lastStatusMs >= 30000) {
    lastStatusMs = millis();
    printStatus();
  }
  
  // Your application code here
  delay(10);
}

void printInstructions() {
  Serial.println();
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("             FIRMWARE UPDATE INSTRUCTIONS");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println();
  Serial.println("1. Open a web browser");
  Serial.print("2. Navigate to: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/update");
  Serial.println("3. Select your firmware .bin file");
  Serial.println("4. Click 'Update' and wait for completion");
  Serial.println();
  
  if (strlen(WEB_USERNAME) > 0) {
    Serial.print("   Username: ");
    Serial.println(WEB_USERNAME);
    Serial.print("   Password: ");
    Serial.println("(configured in secret.h)");
    Serial.println();
  }
  
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println();
}

void printStatus() {
  Serial.print("[Status] WiFi: ");
  Serial.print(otaIsConnected() ? "Connected" : "Disconnected");
  Serial.print(" | Web Server: ");
  Serial.print(otaIsWebServerRunning() ? "Running" : "Stopped");
  Serial.print(" | IP: ");
  Serial.println(WiFi.localIP());
}
