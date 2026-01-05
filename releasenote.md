# Release Notes — PICO_OTA v1.4.2

**Date:** January 5, 2026  
**Type:** Metadata Release

---

## 🎯 Summary

PICO_OTA v1.4.2 is a **metadata-only release** with no API or code changes. This update refreshes copyright metadata to 2026 and updates the copyright holder attribution.

---

## ✨ What's New in v1.4.2

### **1. Copyright Metadata Update** 📝

**Updated copyright information across all source files:**
- LICENSE: Copyright year updated to 2026
- src/pico_ota.h: SPDX header updated to 2026
- src/pico_ota.cpp: SPDX header updated to 2026
- Copyright holder: Samuel F.

**Why this matters:**
- Maintains legal accuracy for current year
- Ensures proper attribution in all distributions
- Keeps library metadata current for Arduino Library Manager

---

## 🔄 Backward Compatibility

✅ **100% backward compatible** - no code changes, only metadata  
✅ All existing sketches work unchanged  
✅ No library behavior changes  
✅ No API additions or modifications  

---

## 📦 Migration from v1.4.1

No code changes required! Simply update your library:
```bash
git pull origin main
```

Or via Arduino Library Manager:
1. Sketch → Include Library → Manage Libraries
2. Search "PICO_OTA"
3. Click Update

---

## 📋 Files Modified

| File | Change |
|------|--------|
| `LICENSE` | Copyright year 2025 → 2026, holder updated |
| `src/pico_ota.h` | SPDX header copyright updated to 2026 |
| `src/pico_ota.cpp` | SPDX header copyright updated to 2026 |
| `library.properties` | Version bump 1.4.1 → 1.4.2 |
| `releasenote.md` | Added v1.4.2 release notes |

---

## 🎓 For Users

This release requires no action on your part. All functionality remains identical to v1.4.1.

---

# Release Notes — PICO_OTA v1.4.1

**Date:** December 14, 2025  
**Type:** Documentation Release

---

## 🎯 Summary

PICO_OTA v1.4.1 is a **documentation-only release** with no API changes. This update provides comprehensive guides for generating `.bin` firmware files and enhances example documentation to match project standards.

---

## ✨ What's New in v1.4.1

### **1. Binary File Generation Guide** 📦

**New comprehensive guide in README.md covering:**
- Arduino IDE method for Pico W and ESP32 (Export Compiled Binary)
- Arduino CLI method for automation and CI/CD pipelines
- Board-specific FQBN configurations
- Output file locations and naming conventions
- Usage instructions for HTTP Pull, Web Browser, and GitHub OTA
- Pro tips and troubleshooting for .bin file deployment

**Why this matters:**
- v1.4.0 added HTTP Pull, Web Browser, and GitHub OTA features
- All three features require `.bin` files
- Many users struggled with "How do I create a .bin file?"
- This guide makes advanced OTA features accessible to all users

### **2. Enhanced Example Documentation** 📚

**ESP32_OTA_test.ino:**
- Expanded from 3 lines to 80+ lines of comprehensive documentation
- Now matches Pico_OTA_test documentation standards
- Includes:
  - Board installation instructions
  - Complete Arduino IDE setup steps
  - Port selection guide for Windows/Mac/Linux
  - BOOT button usage for ESP32 uploads
  - OTA testing workflow with LED blink example
  - Configuration notes for different ESP32 variants

**All examples now consistently documented** with:
- Header block explaining purpose and usage
- Step-by-step setup instructions
- Testing procedures
- Configuration guidelines

### **3. Documentation Improvements** 📝

- README.md: Added .bin generation section after "Testing OTA"
- Improved cross-referencing between examples and features
- Clarified platform-specific requirements (Pico W vs ESP32)
- Enhanced troubleshooting section

---

## 🔄 Backward Compatibility

✅ **100% backward compatible** - no code changes, only documentation
✅ All existing sketches work unchanged
✅ No library behavior changes
✅ No API additions or modifications

---

## 📦 Migration from v1.4.0

No code changes required! Simply update your library:
```bash
git pull origin main
```

Or via Arduino Library Manager:
1. Sketch → Include Library → Manage Libraries
2. Search "PICO_OTA"
3. Click Update

---

## 📋 Files Modified

| File | Change |
|------|--------|
| `README.md` | Added .bin generation guide section (~60 lines) |
| `examples/ESP32_OTA_test/ESP32_OTA_test.ino` | Enhanced documentation header (3 → 80+ lines) |
| `library.properties` | Version bump 1.4.0 → 1.4.1 |
| `releasenote.md` | Added v1.4.1 release notes |

---

## 🎓 For Users

- **New to .bin files?** Check the new guide in README.md after "Testing OTA" section
- **Using ESP32?** Review the enhanced ESP32_OTA_test example for complete setup
- **Production deployment?** Follow the .bin generation CI/CD examples for automation

---

---

# Release Notes — PICO_OTA v1.4.0

**Date:** June 2025  
**Type:** Major Feature Release

---

## 🎯 Summary

PICO_OTA v1.4.0 adds **four major OTA update methods** for production deployments: HTTP Pull-Based OTA, Web Browser Upload, GitHub Release Auto-Update, and WiFi Auto-Reconnect. These features work on both **Raspberry Pi Pico W** and **ESP32** platforms.

---

## ✨ What's New in v1.4.0

### **1. HTTP Pull-Based OTA** 🌐

Download and install firmware updates from any HTTP server.

**API:**
- `int otaUpdateFromUrl(const char* url)` - Download firmware from URL
- `int otaUpdateFromUrl(const char* url, const char* currentVersion)` - With version checking
- `int otaUpdateFromHost(const char* host, uint16_t port, const char* path)` - From host/port/path
- `int otaUpdateFromHost(const char* host, uint16_t port, const char* path, const char* currentVersion)` - With version

**Return Codes:**
- `OTA_UPDATE_OK (0)` - Update successful
- `OTA_UPDATE_NO_UPDATE (1)` - Already running latest version
- `OTA_UPDATE_FAILED (-1)` - Update failed
- `OTA_UPDATE_NO_WIFI (-2)` - No WiFi connection
- `OTA_UPDATE_HTTP_ERROR (-3)` - HTTP error

**Example:** `examples/HTTP_Pull_OTA/`

### **2. Web Browser Upload** 🌍

Start a web server for browser-based firmware uploads. No programming knowledge needed!

**API:**
- `void otaStartWebServer(uint16_t port = 80)` - Start update server
- `void otaStopWebServer()` - Stop server
- `void otaSetWebCredentials(const char* username, const char* password)` - Enable authentication
- `bool otaIsWebServerRunning()` - Check server status

**Usage:** Navigate to `http://<device-ip>/update` and upload your .bin file.

**Example:** `examples/WebBrowser_OTA/`

### **3. GitHub Release Auto-Update** 📦

Automatically check GitHub Releases and update when new versions are available.

**API:**
- `void otaSetGitHubRepo(const char* owner, const char* repo)` - Configure repository
- `void otaSetCurrentVersion(const char* version)` - Set current version
- `void otaSetGitHubAssetName(const char* assetPattern)` - Filter firmware files (e.g., "*.bin")
- `int otaCheckGitHubUpdate(char* latestVersion, size_t maxLen)` - Check for updates
- `int otaUpdateFromGitHub()` - Download and install latest release
- `const char* otaGetLatestGitHubVersion()` - Get latest version string

**Return Codes:**
- `OTA_UPDATE_PARSE_ERROR (-4)` - JSON parse error
- `OTA_UPDATE_NO_ASSET (-5)` - No matching firmware file in release

**Example:** `examples/GitHub_OTA/`

### **4. WiFi Auto-Reconnect** 🔄

Automatically reconnect to WiFi if the connection drops.

**API:**
- `void otaSetAutoReconnect(bool enabled)` - Enable/disable
- `void otaSetReconnectInterval(unsigned long ms)` - Time between attempts (default: 30s)
- `void otaSetMaxReconnectAttempts(int attempts)` - Max retries (0 = infinite)
- `void otaOnWifiDisconnect(void (*callback)())` - Disconnect notification
- `void otaOnWifiReconnect(void (*callback)())` - Reconnect notification

**Example:** All new examples demonstrate auto-reconnect.

---

## 📦 New Examples

- `examples/HTTP_Pull_OTA/` - Pull firmware from HTTP server
- `examples/WebBrowser_OTA/` - Browser-based upload interface
- `examples/GitHub_OTA/` - GitHub Release auto-update

---

## 🔧 Technical Details

- Uses `HTTPUpdate` library for HTTP pull OTA (available on both platforms)
- Uses `WebServer` + `HTTPUpdateServer` for browser uploads
- Uses `HTTPClient` + `WiFiClientSecure` for GitHub API (HTTPS)
- Built-in JSON parser for GitHub API (no ArduinoJson dependency required)
- WiFi credentials stored for auto-reconnect functionality

---

## ⬆️ Migration from v1.3.0

No breaking changes! All existing code continues to work. New features are opt-in.

---

---

# Release Notes — PICO_OTA v1.3.0

**Date:** December 13, 2025  
**Type:** Minor Feature (Reliability improvements + ESP32 support)

---

## 🎯 Summary

PICO_OTA v1.3.0 adds **production-ready reliability features** while maintaining primary focus on **Raspberry Pi Pico W** with optional **ESP32** compatibility. New features include configurable Wi-Fi timeout, OTA callbacks, status queries, and filesystem safety controls to prevent classroom/production deployment issues.

---

## ✨ What's Changed

### **1. Reliability Improvements** 🛡️

#### **Wi-Fi Connection Timeout** ⏱️
- **Issue**: Original `connectWifi()` used infinite loop, blocking forever if WiFi unavailable
- **Solution**: New timeout mechanism (default 30s, configurable via `otaSetWifiTimeout()`)
- **API**: 
  - `void otaSetWifiTimeout(unsigned long timeoutMs)` - Configure timeout before calling `otaSetup()`
  - `bool otaSetupWithTimeout(...)` - Advanced setup with explicit timeout control
- **Benefit**: Prevents classroom "hung boot" scenarios, enables graceful fallback

#### **OTA Callbacks** 📡
- **Issue**: No user feedback during OTA updates
- **Solution**: Expose ArduinoOTA callbacks to user code
- **API**:
  - `void otaOnStart(void (*callback)())` - Called when OTA starts
  - `void otaOnProgress(void (*callback)(unsigned int current, unsigned int total))` - Progress updates
  - `void otaOnEnd(void (*callback)())` - Called when OTA completes
  - `void otaOnError(void (*callback)(int error))` - Error handling
- **Benefit**: Enable LED indicators, serial progress, or custom user feedback

#### **Status Query Functions** 🔍
- **Issue**: No way to check if OTA is ready or WiFi is connected
- **Solution**: New status query functions
- **API**:
  - `bool otaIsConnected()` - Returns true if WiFi is connected
  - `bool otaIsReady()` - Returns true if OTA is fully initialized and ready
- **Benefit**: Enable conditional logic, status displays, and health checks

#### **Filesystem Safety Controls** 💾
- **Issue**: Auto-format on mount failure caused accidental data loss (Pico W only)
- **Solution**: Configurable auto-format behavior
- **API**: `void otaSetFsAutoFormat(bool enabled)` - Default true (beginner-friendly), set false for production
- **Benefit**: Prevent data loss in production deployments while maintaining ease-of-use for beginners

### **2. ESP32 Platform Support (optional)** 🧩

- ✅ `src/pico_ota.h`: Broadened board guard to include `ARDUINO_ARCH_ESP32`
- ✅ `src/pico_ota.cpp`: Conditional LittleFS logic (required only on Pico W); ESP32 OTA path skips filesystem setup
- ✅ `library.properties`: `architectures=rp2040,esp32`
- ✅ `README.md`: Updated to document ESP32 usage and per-target board settings

### **3. New Example: `examples/Non_Blocking_OTA`** 🚀

- Demonstrates all new reliability features:
  - Wi-Fi connection timeout (15s)
  - OTA callbacks for progress feedback
  - Status functions for health monitoring
  - Filesystem auto-format disabled (production safety)
  - LED status indicators (slow blink = connecting, solid = ready, fast blink = failed)
  - Graceful fallback if OTA setup fails
- Shows production-ready patterns for classroom and field deployment

### **4. Existing Example: `examples/ESP32_OTA_test`** 🧪

- Minimal sketch using the same API (`otaSetup`/`otaLoop`)
- Includes a `secret.h` placeholder (SSID, password, hostname, OTA password)
- Optional LED blink to visualize OTA-deployed changes

---

## 🔄 Backward Compatibility

✅ **Existing code continues to work unchanged**
- Original `otaSetup()` and `otaLoop()` API unchanged
- Default behavior preserved (30s timeout, auto-format enabled)
- All existing examples work without modification
- New features are opt-in via additional function calls

---

## 🎓 Notes for Users (Pico W first)

- On **Pico W**: select a Flash Size that includes **LittleFS** (e.g., Sketch: 1MB, FS: 1MB). This is mandatory for OTA staging.
- On **ESP32**: default Flash settings are fine; OTA does not require a filesystem partition.
- Keep credentials in `secret.h`; do not commit secrets.
- **Production deployments**: Call `otaSetFsAutoFormat(false)` before `otaSetup()` to prevent accidental data loss
- **Unreliable WiFi environments**: Call `otaSetWifiTimeout(15000)` for faster failover

---

## 📋 Files Modified / Added

| File | Change |
|------|--------|
| `src/pico_ota.h` | Add ESP32 support in board guard |
| `src/pico_ota.cpp` | Conditional FS logic (Pico W only), shared Wi-Fi/OTA setup |
| `library.properties` | Architectures broadened; version 1.3.0 |
| `README.md` | Document ESP32 usage and settings |
| `examples/ESP32_OTA_test/ESP32_OTA_test.ino` | New example for ESP32 |
| `examples/ESP32_OTA_test/secret.h` | Credentials placeholder |

---

## ✅ Benefits (unchanged API)

- 🧩 Cross-platform: Pico W and optional ESP32 share the same simple OTA API.
- 🔒 Security hygiene: keep credentials in `secret.h`.
- 🚀 Quick setup: ESP32 requires no FS partition; Pico W remains straightforward with LittleFS.

---

## 🔄 Backward Compatibility

✅ **API unchanged** (existing Pico W projects continue to work)
✅ Examples compile for both platforms

---

## 📦 How to Update

Pull the latest code and select your board in Arduino IDE:
```bash
git pull origin main
```

---

## 🙏 Notes

This is a **feature release** adding ESP32 support while keeping the API consistent and the Pico W workflow intact.

