#include "pico_ota.h"

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <HTTPUpdateServer.h>

#if defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_RASPBERRY_PI_PICO2_W)
#include <LittleFS.h>
#include <HTTPUpdate.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <HTTPUpdate.h>
#endif

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Static configuration & state
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
static unsigned long g_wifiTimeoutMs = 30000;  // Default: 30s
static bool g_fsAutoFormat = true;             // Default: true (Pico W only)
static bool g_otaStarted = false;              // Tracks if ArduinoOTA.begin() was called

// WiFi credentials storage for reconnect
static String g_ssid;
static String g_password;
static String g_hostname;

// WiFi Auto-Reconnect settings
static bool g_autoReconnect = false;
static unsigned long g_reconnectInterval = 30000;  // Default: 30s
static int g_maxReconnectAttempts = 0;  // 0 = infinite
static int g_reconnectAttempts = 0;
static unsigned long g_lastReconnectAttempt = 0;
static bool g_wasConnected = false;

// User callbacks (optional)
static void (*g_onStartCallback)() = nullptr;
static void (*g_onProgressCallback)(unsigned int, unsigned int) = nullptr;
static void (*g_onEndCallback)() = nullptr;
static void (*g_onErrorCallback)(int) = nullptr;
static void (*g_onWifiDisconnectCallback)() = nullptr;
static void (*g_onWifiReconnectCallback)() = nullptr;

// Web Server for browser upload
static WebServer* g_webServer = nullptr;
static HTTPUpdateServer* g_httpUpdater = nullptr;
static bool g_webServerRunning = false;
static String g_webUsername;
static String g_webPassword;
static uint16_t g_webServerPort = 80;

// GitHub OTA settings
static String g_githubOwner;
static String g_githubRepo;
static String g_currentVersion;
static String g_githubAssetPattern;
static String g_latestVersion;
static String g_latestAssetUrl;

namespace {

#if defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_RASPBERRY_PI_PICO2_W)
bool ensureLittleFsMounted() {
  if (LittleFS.begin()) {
    Serial.println("[OTA] LittleFS mounted");
    return true;
  }

  if (!g_fsAutoFormat) {
    Serial.println("[OTA] LittleFS mount failed (auto-format disabled)");
    return false;
  }
  
  Serial.println("[OTA] LittleFS mount failed, trying to format...");
  if (LittleFS.format() == true && LittleFS.begin() == true) {
    Serial.println("[OTA] LittleFS formatted and mounted");
    return true;
  }

  Serial.println("[OTA] ERROR: LittleFS unavailable (check Flash Size partition includes FS)");
  return false;
}
#endif

bool connectWifi(const char *ssid, const char *password, unsigned long timeoutMs) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("[OTA] Connecting WiFi");
  
  unsigned long startMs = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startMs > timeoutMs) {
      Serial.println();
      Serial.println("[OTA] WiFi connection timeout");
      return false;
    }
    Serial.print('.');
    delay(500);
  }
  Serial.println();
  return true;
}
}  // namespace

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Configuration setters
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void otaSetWifiTimeout(unsigned long timeoutMs) {
  g_wifiTimeoutMs = timeoutMs;
}

void otaSetFsAutoFormat(bool enabled) {
  g_fsAutoFormat = enabled;
}

void otaOnStart(void (*callback)()) {
  g_onStartCallback = callback;
}

void otaOnProgress(void (*callback)(unsigned int, unsigned int)) {
  g_onProgressCallback = callback;
}

void otaOnEnd(void (*callback)()) {
  g_onEndCallback = callback;
}

void otaOnError(void (*callback)(int)) {
  g_onErrorCallback = callback;
}
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Setup helpers
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
static void configureArduinoOTA(const char *hostname, const char *otaPassword) {
  // Set callbacks if provided
  if (g_onStartCallback) {
    ArduinoOTA.onStart(g_onStartCallback);
  }
  if (g_onProgressCallback) {
    ArduinoOTA.onProgress(g_onProgressCallback);
  }
  if (g_onEndCallback) {
    ArduinoOTA.onEnd(g_onEndCallback);
  }
  if (g_onErrorCallback) {
    ArduinoOTA.onError([](ota_error_t error) {
      g_onErrorCallback((int)error);
    });
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
  
  Serial.println("[OTA] Starting ArduinoOTA...");
  ArduinoOTA.begin();
  g_otaStarted = true;
  Serial.println("[OTA] Ready for OTA updates");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Public API: basic setup (backward compatible)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void otaSetup(const char *ssid,
              const char *password,
              const char *hostname,
              const char *otaPassword) {
  otaSetupWithTimeout(ssid, password, g_wifiTimeoutMs, hostname, otaPassword, g_fsAutoFormat);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Public API: advanced setup with timeout and FS control
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
bool otaSetupWithTimeout(const char *ssid,
                         const char *password,
                         unsigned long wifiTimeoutMs,
                         const char *hostname,
                         const char *otaPassword,
                         bool allowFsFormat) {
  // Store credentials for auto-reconnect
  g_ssid = ssid ? ssid : "";
  g_password = password ? password : "";
  g_hostname = hostname ? hostname : "";
  
  // Temporarily override FS auto-format for this setup call
  bool originalFsAutoFormat = g_fsAutoFormat;
  g_fsAutoFormat = allowFsFormat;
  
  if (!connectWifi(ssid, password, wifiTimeoutMs)) {
    Serial.println("[OTA] OTA disabled because WiFi connection failed");
    g_fsAutoFormat = originalFsAutoFormat;  // Restore
    return false;
  }
  
  g_wasConnected = true;  // Mark as connected for auto-reconnect
  
  Serial.print("[OTA] WiFi connected, IP: ");
  Serial.println(WiFi.localIP());

#if defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_RASPBERRY_PI_PICO2_W)
  // RP2040 Pico W uses LittleFS to stage OTA updates; ensure it is available.
  if (!ensureLittleFsMounted()) {
    Serial.println("[OTA] OTA disabled because filesystem is missing");
    g_fsAutoFormat = originalFsAutoFormat;  // Restore
    return false;
  }
#endif

  configureArduinoOTA(hostname, otaPassword);
  g_fsAutoFormat = originalFsAutoFormat;  // Restore
  return true;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// WiFi Auto-Reconnect
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void otaSetAutoReconnect(bool enabled) {
  g_autoReconnect = enabled;
  if (enabled) {
    g_reconnectAttempts = 0;
    g_lastReconnectAttempt = 0;
  }
}

void otaSetReconnectInterval(unsigned long ms) {
  g_reconnectInterval = ms;
}

void otaSetMaxReconnectAttempts(int attempts) {
  g_maxReconnectAttempts = attempts;
}

void otaOnWifiDisconnect(void (*callback)()) {
  g_onWifiDisconnectCallback = callback;
}

void otaOnWifiReconnect(void (*callback)()) {
  g_onWifiReconnectCallback = callback;
}

static void handleAutoReconnect() {
  if (!g_autoReconnect) return;
  
  bool currentlyConnected = (WiFi.status() == WL_CONNECTED);
  
  // Detect disconnect
  if (g_wasConnected && !currentlyConnected) {
    g_wasConnected = false;
    g_reconnectAttempts = 0;
    Serial.println("[OTA] WiFi disconnected");
    if (g_onWifiDisconnectCallback) {
      g_onWifiDisconnectCallback();
    }
  }
  
  // Attempt reconnect if needed
  if (!currentlyConnected && g_ssid.length() > 0) {
    // Check if max attempts reached
    if (g_maxReconnectAttempts > 0 && g_reconnectAttempts >= g_maxReconnectAttempts) {
      return;
    }
    
    // Check if interval has passed
    if (millis() - g_lastReconnectAttempt < g_reconnectInterval) {
      return;
    }
    
    g_lastReconnectAttempt = millis();
    g_reconnectAttempts++;
    
    Serial.print("[OTA] Reconnect attempt ");
    Serial.print(g_reconnectAttempts);
    if (g_maxReconnectAttempts > 0) {
      Serial.print("/");
      Serial.print(g_maxReconnectAttempts);
    }
    Serial.println();
    
    WiFi.disconnect();
    delay(100);
    WiFi.begin(g_ssid.c_str(), g_password.c_str());
    
    // Brief wait for connection
    unsigned long connectStart = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - connectStart < 10000) {
      delay(100);
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      g_wasConnected = true;
      g_reconnectAttempts = 0;
      Serial.print("[OTA] Reconnected, IP: ");
      Serial.println(WiFi.localIP());
      if (g_onWifiReconnectCallback) {
        g_onWifiReconnectCallback();
      }
    }
  }
  
  // Update connected state
  if (currentlyConnected && !g_wasConnected) {
    g_wasConnected = true;
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Runtime loop
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void otaLoop() {
  ArduinoOTA.handle();
  handleAutoReconnect();
  
  // Handle web server if running
  if (g_webServerRunning && g_webServer) {
    g_webServer->handleClient();
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Status queries
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
bool otaIsConnected() {
  return WiFi.status() == WL_CONNECTED;
}

bool otaIsReady() {
  return (WiFi.status() == WL_CONNECTED) && g_otaStarted;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// HTTP Pull-Based OTA
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
int otaUpdateFromUrl(const char* url) {
  return otaUpdateFromUrl(url, "");
}

int otaUpdateFromUrl(const char* url, const char* currentVersion) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[OTA] HTTP update failed: WiFi not connected");
    return OTA_UPDATE_NO_WIFI;
  }
  
  Serial.print("[OTA] Starting HTTP update from: ");
  Serial.println(url);
  
  WiFiClient client;
  
  // Both platforms use lowercase 'httpUpdate'
  httpUpdate.rebootOnUpdate(true);
  t_httpUpdate_return ret;
  if (currentVersion && *currentVersion) {
    ret = httpUpdate.update(client, url, currentVersion);
  } else {
    ret = httpUpdate.update(client, url);
  }
  
  switch (ret) {
    case HTTP_UPDATE_OK:
      Serial.println("[OTA] HTTP update successful, rebooting...");
      return OTA_UPDATE_OK;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("[OTA] No update available (version match)");
      return OTA_UPDATE_NO_UPDATE;
    case HTTP_UPDATE_FAILED:
    default:
      Serial.printf("[OTA] HTTP update failed: %s\n", httpUpdate.getLastErrorString().c_str());
      return OTA_UPDATE_FAILED;
  }
}

int otaUpdateFromHost(const char* host, uint16_t port, const char* path) {
  return otaUpdateFromHost(host, port, path, "");
}

int otaUpdateFromHost(const char* host, uint16_t port, const char* path, const char* currentVersion) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[OTA] HTTP update failed: WiFi not connected");
    return OTA_UPDATE_NO_WIFI;
  }
  
  Serial.printf("[OTA] Starting HTTP update from: %s:%d%s\n", host, port, path);
  
  WiFiClient client;
  
  // Both platforms use lowercase 'httpUpdate'
  httpUpdate.rebootOnUpdate(true);
  t_httpUpdate_return ret;
  if (currentVersion && *currentVersion) {
    ret = httpUpdate.update(client, host, port, path, currentVersion);
  } else {
    ret = httpUpdate.update(client, host, port, path);
  }

  switch (ret) {
    case HTTP_UPDATE_OK:
      Serial.println("[OTA] HTTP update successful, rebooting...");
      return OTA_UPDATE_OK;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("[OTA] No update available (version match)");
      return OTA_UPDATE_NO_UPDATE;
    default:
      return OTA_UPDATE_FAILED;
  }
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Web Browser Upload
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void otaSetWebCredentials(const char* username, const char* password) {
  g_webUsername = username ? username : "";
  g_webPassword = password ? password : "";
}

void otaStartWebServer(uint16_t port) {
  if (g_webServerRunning) {
    Serial.println("[OTA] Web server already running");
    return;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[OTA] Cannot start web server: WiFi not connected");
    return;
  }
  
  g_webServerPort = port;
  
  // Create server and updater
  g_webServer = new WebServer(port);
  g_httpUpdater = new HTTPUpdateServer();
  
  // Setup HTTPUpdateServer with optional authentication
  if (g_webUsername.length() > 0 && g_webPassword.length() > 0) {
    g_httpUpdater->setup(g_webServer, "/update", g_webUsername.c_str(), g_webPassword.c_str());
  } else {
    g_httpUpdater->setup(g_webServer, "/update");
  }
  
  // Root page with link to update
  g_webServer->on("/", HTTP_GET, []() {
    String html = "<!DOCTYPE html><html><head>";
    html += "<title>OTA Update</title>";
    html += "<style>body{font-family:Arial,sans-serif;margin:40px;background:#f0f0f0;}";
    html += ".container{background:white;padding:30px;border-radius:10px;max-width:500px;margin:auto;box-shadow:0 2px 10px rgba(0,0,0,0.1);}";
    html += "h1{color:#333;text-align:center;}";
    html += "a{display:block;text-align:center;padding:15px 30px;background:#007bff;color:white;text-decoration:none;border-radius:5px;margin-top:20px;}";
    html += "a:hover{background:#0056b3;}</style></head>";
    html += "<body><div class='container'>";
    html += "<h1>Pico OTA Update</h1>";
    html += "<p style='text-align:center;'>Device IP: " + WiFi.localIP().toString() + "</p>";
    html += "<a href='/update'>Go to Firmware Update</a>";
    html += "</div></body></html>";
    g_webServer->send(200, "text/html", html);
  });
  
  g_webServer->begin();
  g_webServerRunning = true;
  
  Serial.print("[OTA] Web server started on port ");
  Serial.println(port);
  Serial.print("[OTA] Update URL: http://");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.print(port);
  Serial.println("/update");
}

void otaStopWebServer() {
  if (!g_webServerRunning) {
    return;
  }
  
  if (g_webServer) {
    g_webServer->stop();
    delete g_webServer;
    g_webServer = nullptr;
  }
  
  if (g_httpUpdater) {
    delete g_httpUpdater;
    g_httpUpdater = nullptr;
  }
  
  g_webServerRunning = false;
  Serial.println("[OTA] Web server stopped");
}

bool otaIsWebServerRunning() {
  return g_webServerRunning;
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// GitHub Release OTA
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void otaSetGitHubRepo(const char* owner, const char* repo) {
  g_githubOwner = owner ? owner : "";
  g_githubRepo = repo ? repo : "";
}

void otaSetCurrentVersion(const char* version) {
  g_currentVersion = version ? version : "";
}

void otaSetGitHubAssetName(const char* assetPattern) {
  g_githubAssetPattern = assetPattern ? assetPattern : "";
}

const char* otaGetLatestGitHubVersion() {
  return g_latestVersion.c_str();
}

// Simple JSON parser helper (avoids ArduinoJson dependency)
static String extractJsonValue(const String& json, const char* key) {
  String searchKey = String("\"") + key + "\"";
  int keyPos = json.indexOf(searchKey);
  if (keyPos < 0) return "";
  
  int colonPos = json.indexOf(':', keyPos + searchKey.length());
  if (colonPos < 0) return "";
  
  // Skip whitespace
  int valueStart = colonPos + 1;
  while (valueStart < (int)json.length() && (json[valueStart] == ' ' || json[valueStart] == '\t')) {
    valueStart++;
  }
  
  if (valueStart >= (int)json.length()) return "";
  
  // Check if value is a string (starts with ")
  if (json[valueStart] == '"') {
    valueStart++;
    int valueEnd = json.indexOf('"', valueStart);
    if (valueEnd < 0) return "";
    return json.substring(valueStart, valueEnd);
  }
  
  // Otherwise it's a number or boolean - find end
  int valueEnd = valueStart;
  while (valueEnd < (int)json.length() && json[valueEnd] != ',' && json[valueEnd] != '}') {
    valueEnd++;
  }
  return json.substring(valueStart, valueEnd);
}

// Find asset URL from GitHub release JSON
static String findAssetUrl(const String& json, const String& pattern) {
  // Look for "assets" array
  int assetsPos = json.indexOf("\"assets\"");
  if (assetsPos < 0) return "";
  
  int arrayStart = json.indexOf('[', assetsPos);
  if (arrayStart < 0) return "";
  
  // Search through assets
  int searchPos = arrayStart;
  while (searchPos < (int)json.length()) {
    int namePos = json.indexOf("\"name\"", searchPos);
    if (namePos < 0) break;
    
    // Extract asset name
    int colonPos = json.indexOf(':', namePos + 6);
    if (colonPos < 0) break;
    
    int quoteStart = json.indexOf('"', colonPos + 1);
    int quoteEnd = json.indexOf('"', quoteStart + 1);
    if (quoteStart < 0 || quoteEnd < 0) break;
    
    String assetName = json.substring(quoteStart + 1, quoteEnd);
    
    // Check if matches pattern (simple wildcard support)
    bool matches = false;
    if (pattern.length() == 0) {
      // No pattern - match .bin files
      matches = assetName.endsWith(".bin");
    } else if (pattern.indexOf('*') >= 0) {
      // Simple wildcard pattern
      String prefix = pattern.substring(0, pattern.indexOf('*'));
      String suffix = pattern.substring(pattern.indexOf('*') + 1);
      matches = assetName.startsWith(prefix) && assetName.endsWith(suffix);
    } else {
      matches = (assetName == pattern);
    }
    
    if (matches) {
      // Find browser_download_url for this asset
      int urlPos = json.indexOf("\"browser_download_url\"", namePos);
      if (urlPos >= 0) {
        colonPos = json.indexOf(':', urlPos + 22);
        quoteStart = json.indexOf('"', colonPos + 1);
        quoteEnd = json.indexOf('"', quoteStart + 1);
        if (quoteStart >= 0 && quoteEnd >= 0) {
          return json.substring(quoteStart + 1, quoteEnd);
        }
      }
    }
    
    searchPos = quoteEnd + 1;
  }
  
  return "";
}

int otaCheckGitHubUpdate(char* latestVersion, size_t maxLen) {
  if (WiFi.status() != WL_CONNECTED) {
    return OTA_UPDATE_NO_WIFI;
  }
  
  if (g_githubOwner.length() == 0 || g_githubRepo.length() == 0) {
    Serial.println("[OTA] GitHub repo not configured");
    return OTA_UPDATE_FAILED;
  }
  
  HTTPClient http;
  WiFiClientSecure client;
  client.setInsecure();  // Skip certificate verification
  
  String url = "https://api.github.com/repos/" + g_githubOwner + "/" + g_githubRepo + "/releases/latest";
  
  Serial.print("[OTA] Checking GitHub releases: ");
  Serial.println(url);
  
  http.begin(client, url);
  http.addHeader("User-Agent", "Pico-OTA");
  http.addHeader("Accept", "application/vnd.github.v3+json");
  
  int httpCode = http.GET();
  
  if (httpCode != 200) {
    Serial.printf("[OTA] GitHub API error: %d\n", httpCode);
    http.end();
    return OTA_UPDATE_HTTP_ERROR;
  }
  
  String payload = http.getString();
  http.end();
  
  // Parse tag_name for version
  g_latestVersion = extractJsonValue(payload, "tag_name");
  if (g_latestVersion.length() == 0) {
    Serial.println("[OTA] Failed to parse version from GitHub response");
    return OTA_UPDATE_PARSE_ERROR;
  }
  
  // Remove 'v' prefix if present
  if (g_latestVersion.startsWith("v") || g_latestVersion.startsWith("V")) {
    g_latestVersion = g_latestVersion.substring(1);
  }
  
  Serial.print("[OTA] Latest GitHub version: ");
  Serial.println(g_latestVersion);
  
  // Copy to output if provided
  if (latestVersion && maxLen > 0) {
    strncpy(latestVersion, g_latestVersion.c_str(), maxLen - 1);
    latestVersion[maxLen - 1] = '\0';
  }
  
  // Find download URL for firmware asset
  g_latestAssetUrl = findAssetUrl(payload, g_githubAssetPattern);
  if (g_latestAssetUrl.length() == 0) {
    Serial.println("[OTA] No matching firmware asset found in release");
    return OTA_UPDATE_NO_ASSET;
  }
  
  Serial.print("[OTA] Asset URL: ");
  Serial.println(g_latestAssetUrl);
  
  // Compare versions
  if (g_currentVersion.length() > 0 && g_currentVersion == g_latestVersion) {
    Serial.println("[OTA] Already running latest version");
    return OTA_UPDATE_NO_UPDATE;
  }
  
  return OTA_UPDATE_OK;  // Update available
}

int otaUpdateFromGitHub() {
  // Check for update first
  int checkResult = otaCheckGitHubUpdate(nullptr, 0);
  
  if (checkResult == OTA_UPDATE_NO_UPDATE) {
    return OTA_UPDATE_NO_UPDATE;
  }
  
  if (checkResult != OTA_UPDATE_OK) {
    return checkResult;
  }
  
  if (g_latestAssetUrl.length() == 0) {
    return OTA_UPDATE_NO_ASSET;
  }
  
  Serial.println("[OTA] Starting GitHub OTA update...");
  
  // Download and install
  return otaUpdateFromUrl(g_latestAssetUrl.c_str(), g_currentVersion.c_str());
}
