// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Samuel F.

#pragma once

#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP32)
	// Supported board: ESP32 (ESP32 core)
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_RASPBERRY_PI_PICO2_W)
	// Supported board: Raspberry Pi Pico W / Pico 2 W (Arduino-Pico core)
#else
	#error "Supported boards: ESP32 and Raspberry Pi Pico W/Pico 2 W (Wi‑Fi)."
#endif

// OTA helper API (ESP32 and RP2040 Pico W)
// - Call otaSetup(...) once during startup (after Serial.begin if you want logs).
// - Call otaLoop() frequently (e.g., every loop iteration) to keep OTA alive.
// - Prefer to keep real Wi-Fi credentials out of source control (e.g., secrets.h in .gitignore).

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// OTA Update Result Codes
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
enum OtaUpdateResult {
    OTA_UPDATE_OK = 0,              // Update successful, device will reboot
    OTA_UPDATE_NO_UPDATE = 1,       // No update available (version check)
    OTA_UPDATE_FAILED = -1,         // Update failed (download/write error)
    OTA_UPDATE_NO_WIFI = -2,        // WiFi not connected
    OTA_UPDATE_HTTP_ERROR = -3,     // HTTP request failed
    OTA_UPDATE_PARSE_ERROR = -4,    // Failed to parse response (GitHub JSON)
    OTA_UPDATE_NO_ASSET = -5        // No suitable firmware asset found
};

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Configuration (call before otaSetup)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void otaSetWifiTimeout(unsigned long timeoutMs);    // Default: 30000ms (30s)
void otaSetFsAutoFormat(bool enabled);              // Default: true (Pico W only)

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// WiFi Auto-Reconnect (optional, call before otaSetup)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void otaSetAutoReconnect(bool enabled);                   // Default: false
void otaSetReconnectInterval(unsigned long ms);           // Default: 30000ms
void otaSetMaxReconnectAttempts(int attempts);            // Default: 0 (infinite)
void otaOnWifiDisconnect(void (*callback)());             // Called when WiFi drops
void otaOnWifiReconnect(void (*callback)());              // Called when WiFi reconnects

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Callbacks (optional, call before otaSetup)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void otaOnStart(void (*callback)());                              // Called when OTA starts
void otaOnProgress(void (*callback)(unsigned int, unsigned int)); // (current, total)
void otaOnEnd(void (*callback)());                                // Called when OTA ends
void otaOnError(void (*callback)(int));                           // Called on OTA error (ota_error_t code)

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Setup (call once in setup())
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Basic setup (uses default timeout and FS auto-format)
void otaSetup(const char *ssid,
			  const char *password,
			  const char *hostname = nullptr,
			  const char *otaPassword = nullptr);

// Advanced setup with explicit timeout and FS control
bool otaSetupWithTimeout(const char *ssid,
						 const char *password,
						 unsigned long wifiTimeoutMs,
						 const char *hostname = nullptr,
						 const char *otaPassword = nullptr,
						 bool allowFsFormat = true);

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Runtime (call in loop())
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void otaLoop();

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Status queries
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
bool otaIsConnected();  // Returns true if Wi-Fi is connected
bool otaIsReady();      // Returns true if OTA is ready (Wi-Fi connected + OTA started)

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// HTTP Pull-Based OTA (download firmware from URL)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Simple update from URL (no version check)
int otaUpdateFromUrl(const char* url);

// Update with version check (sends currentVersion to server)
int otaUpdateFromUrl(const char* url, const char* currentVersion);

// Update from IP:port/path
int otaUpdateFromHost(const char* host, uint16_t port, const char* path);
int otaUpdateFromHost(const char* host, uint16_t port, const char* path, const char* currentVersion);

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Web Browser Upload Server
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void otaStartWebServer(uint16_t port = 80);        // Start HTTP update server
void otaStopWebServer();                            // Stop HTTP update server
void otaSetWebCredentials(const char* username,     // Set HTTP Basic Auth (optional)
                          const char* password);
bool otaIsWebServerRunning();                       // Check if web server is active

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// GitHub Release OTA
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void otaSetGitHubRepo(const char* owner, const char* repo);  // e.g., "wedsamuel1230", "PICO_OTA"
void otaSetCurrentVersion(const char* version);               // e.g., "1.3.0"
void otaSetGitHubAssetName(const char* assetPattern);        // e.g., "firmware.bin" or "pico_w.bin"

int otaCheckGitHubUpdate(char* latestVersion = nullptr, size_t maxLen = 0);  // Check for updates
int otaUpdateFromGitHub();                                                    // Download and install
const char* otaGetLatestGitHubVersion();                                     // Get latest version string
