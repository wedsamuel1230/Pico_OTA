# 🚀 PICO_OTA — Wireless Arduino Updates for Raspberry Pi Pico W (ESP32 optional)

<div align="center">

**Upload code wirelessly to your Raspberry Pi Pico W — no USB cable needed!**
ESP32 support is available using the same API, but Pico W remains the primary focus.

[![Arduino](https://img.shields.io/badge/Arduino-Compatible-00979D?style=flat&logo=arduino)](https://www.arduino.cc/)
[![Platform](https://img.shields.io/badge/Platform-RP2040-C51A4A?style=flat)](https://www.raspberrypi.com/documentation/microcontrollers/rp2040.html)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

</div>

---

## 📖 What is This?

A simple Arduino library that enables **Over-The-Air (OTA)** updates for **Raspberry Pi Pico W** (Arduino-Pico core). Optional ESP32 support uses the same API.

**How it works:**
1. 📡 Upload your sketch once via USB - it connects to Wi-Fi and starts OTA server
2. ✨ After that, upload wirelessly from Arduino IDE - no more plugging/unplugging!
3. 💡 Test it by uncommenting LED blink code and uploading via OTA

---

## 📋 What You Need

| Requirement | Details |
|------------|---------|
| 🎛️ **Board** | Raspberry Pi Pico W / Pico 2 W (primary), ESP32 (optional) |
| 💻 **Software** | Arduino IDE 1.8.x / 2.x + [Arduino-Pico core](https://github.com/earlephilhower/arduino-pico) |
| 💾 **Flash Config** | A partition with LittleFS filesystem (e.g., 2MB Sketch + 1MB FS) |
| 📶 **Network** | Device and computer on the same Wi-Fi network |

---

## ⚡ Quick Start Guide

### Step 1️⃣: Install Arduino-Pico Core

1. Open Arduino IDE
2. Go to **`File → Preferences → Additional Board Manager URLs`**
3. Add this URL:
   ```
   https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
   ```
4. Go to **`Tools → Board → Boards Manager`**
5. Search for **"pico"** and install **"Raspberry Pi Pico/RP2040/2350 by Earle F. Philhower, III"**

---

### Step 2️⃣: Configure Board Settings ⚠️ CRITICAL (Pico W focus)

| Setting | Value | Why? |
|---------|-------|------|
| **Board (Pico W)** | `Tools → Board → Raspberry Pi RP2040 Boards →`<br>`Raspberry Pi Pico W` (or Pico 2 W) | Selects your hardware |
| **Flash Size (Pico W)** | `Tools → Flash Size →`<br>`2MB (Sketch: 1MB, FS: 1MB)` ✅ | OTA needs LittleFS space to stage updates |
| **Board (ESP32, optional)** | `Tools → Board → ESP32 Arduino → your ESP32 board` | ESP32 support uses the same API |
| **Flash Size (ESP32, optional)** | Default is fine | ESP32 OTA does not require a filesystem partition |
| **Port** | `Tools → Port →`<br>Windows: `COMx` (e.g., COM3)<br>Mac/Linux: `/dev/ttyACM0` or `/dev/cu.usbmodem*` | For USB upload |

> ⚠️ **Pico W:** DO NOT select "2MB (No FS)" — OTA will fail without filesystem space!

---

### Step 3️⃣: Configure Wi-Fi Credentials

**Both examples have Wi-Fi credentials in `secret.h` (already set up for you!):**

1. Open the example's `secret.h` file:
   - Single-core: `examples/Pico_OTA_test/secret.h`
   - Dual-core: `examples/Pico_OTA_test_with_Dual_Core/secret.h`
2. Edit these lines with your Wi-Fi and OTA settings:
   ```cpp
   const char *ssid = "Your_SSID";        // ← Your Wi-Fi network name
   const char *password = "Your_PASSWORD"; // ← Your Wi-Fi password
   const char *hostname = "pico-ota";      // ← Device name (shows in network ports)
   const char *otaPassword = "admin";      // ← OTA upload password
   ```

> 💡 **Why `secret.h`?** This file is in `.gitignore`, so your Wi-Fi credentials stay secure and never get committed to git!

---

### Step 4️⃣: First Upload (USB) 🔌

1. Connect Pico W to your computer via USB cable
2. Select your USB port: **`Tools → Port → COMx`** (or `/dev/ttyACM0` on Linux/Mac)
3. Click **Upload** button (or press `Ctrl+U` / `Cmd+U`)
4. Open **Serial Monitor** (`115200 baud`)
5. 📝 **Note the IP address** displayed (e.g., `192.168.1.100`)

**You should see something like:**
```
[OTA] Connecting WiFi.....
[OTA] WiFi connected, IP: 192.168.1.100
[OTA] LittleFS mounted
[OTA] Ready for OTA updates
```

---

### Step 5️⃣: Wireless OTA Uploads 🎉

1. Go to **`Tools → Port → Network Ports`**
2. You'll see your device: **`pico-ota at 192.168.1.100`** (or your custom hostname)
3. Select it
4. Click **Upload** - it uploads wirelessly! 🚀

**No USB cable needed anymore!**

---

## 🚀 Advanced: Dual-Core OTA (Better Performance)

For more demanding applications, use the **dual-core example** to dedicate Core 1 exclusively to OTA while Core 0 runs your main application without blocking:

**Main sketch (`Pico_OTA_test_with_Dual_Core.ino`):**
```cpp
#include "secret.h"  // Contains: ssid, password, hostname, otaPassword
                     // Also contains setup1() and loop1() for Core 1 OTA server

// Core 0 setup - YOUR application setup code
void setup() {
  Serial.begin(115200);
  // Your setup code here
}

// Core 0 loop - YOUR main application (never blocks for OTA!)
void loop() {
  // Your responsive application code
  // Sensors, motors, real-time tasks - never blocked by OTA!
  delay(100);
}
```

**OTA Server in `secret.h` (pre-configured, don't modify):**
```cpp
// Wi-Fi credentials (EDIT THESE)
const char *ssid = "Your_SSID";
const char *password = "Your_PASSWORD";
const char *hostname = "pico-ota";
const char *otaPassword = "admin";

// Core 1 setup - initializes OTA on Core 1 (automatic)
void setup1() {
  Serial.println("[OTA] Core 1 OTA server initializing...");
  otaSetup(ssid, password, hostname, otaPassword);
  Serial.println("[OTA] Core 1 OTA ready - waiting for uploads");
}

// Core 1 loop - runs OTA server on Core 1 (automatic)
void loop1() {
  otaLoop();  // Handles OTA independently
}
```

**Benefits:**
- ✅ Core 0: Dedicated to your application (sensors, motors, real-time tasks)
- ✅ Core 1: Dedicated to OTA (never interferes with your code)
- ✅ Better responsiveness and performance
- ✅ True parallel processing on dual-core RP2040

**To use:** Open example **`File → Examples → PICO_OTA → Pico_OTA_test_with_Dual_Core`**

---

## 🧪 Testing OTA (LED Blink Example)

Want to verify OTA is really working? Add an LED blink test to your sketch:

1. ✅ Upload the example via USB first (LED code is commented out by default)
2. ✏️ In the **main sketch**, uncomment the LED variables at the top:
   ```cpp
   // const int ledPin = LED_BUILTIN;
   // unsigned long lastBlink = 0;
   // const unsigned long blinkIntervalMs = 500;
   ```
3. ✏️ In the **loop()** function, uncomment the LED blink logic:
   ```cpp
   // const unsigned long now = millis();
   // if (now - lastBlink >= blinkIntervalMs) {
   //   lastBlink = now;
   //   digitalWrite(ledPin, !digitalRead(ledPin));
   // }
   ```
4. 📡 Upload again via **OTA (wireless port)** - do NOT use USB
5. 💡 The LED starts blinking - **OTA works!**

> 🎯 **Why this works:** You uncommented code that blinks the LED. If you can upload it wirelessly (OTA) and see the LED blink, that proves OTA successfully updated your device!

---

## � Generating .bin Files for OTA Updates

For HTTP Pull OTA, Web Browser Upload, and GitHub Releases, you need to generate `.bin` firmware files. Here's how:

### Method 1: Arduino IDE (Recommended for Beginners) 🖱️

**Pico W:**
1. Open your sketch in Arduino IDE
2. Select **`Tools → Board → Raspberry Pi RP2040 Boards → Raspberry Pi Pico W`**
3. Select **`Tools → Flash Size → 2MB (Sketch: 1MB, FS: 1MB)`**
4. Go to **`Sketch → Export Compiled Binary`** (or press `Ctrl+Alt+S` / `Cmd+Alt+S`)
5. ✅ IDE compiles and creates `.bin` file in your sketch folder
6. 📁 Find it at: `<sketch_folder>/<sketch_name>.ino.bin`

**ESP32:**
1. Select **`Tools → Board → ESP32 Arduino → <your ESP32 board>`**
2. Go to **`Sketch → Export Compiled Binary`** (or press `Ctrl+Alt+S` / `Cmd+Alt+S`)
3. ✅ IDE creates `.bin` file in sketch folder
4. 📁 Find it at: `<sketch_folder>/<sketch_name>.ino.bin`

**Example:**
If your sketch is `MyProject.ino` in `D:\Arduino\MyProject\`, the binary will be:
```
D:\Arduino\MyProject\MyProject.ino.bin
```

### Method 2: Arduino CLI (For Automation & CI/CD) ⚙️

**Pico W:**
```bash
arduino-cli compile --fqbn rp2040:rp2040:rpipicow \
  --export-binaries \
  --build-property "build.partitions=default_8MB" \
  MyProject/
```

Output: `MyProject/build/rp2040.rp2040.rpipicow/MyProject.ino.bin`

**ESP32:**
```bash
arduino-cli compile --fqbn esp32:esp32:esp32 \
  --export-binaries \
  MyProject/
```

Output: `MyProject/build/esp32.esp32.esp32/MyProject.ino.bin`

### Using .bin Files 🚀

**For HTTP Pull OTA:**
1. Upload `.bin` to your web server
2. Configure firmware URL in sketch: `const char* FIRMWARE_URL = "http://your-server.com/firmware.bin";`
3. Device downloads and installs automatically

**For Web Browser OTA:**
1. Navigate to `http://<device-ip>/update` in browser
2. Click "Choose File" and select your `.bin`
3. Click "Update" - device installs and reboots

**For GitHub Releases:**
1. Create a new Release on GitHub (e.g., tag `v1.1.0`)
2. Attach your `.bin` file to the release
3. Device checks GitHub API and downloads new version automatically

**Pro Tips:**
- 💡 Rename `.bin` files with version numbers: `firmware-v1.2.0.bin`
- 💡 For GitHub, use consistent naming: `firmware-pico.bin`, `firmware-esp32.bin`
- 💡 Test `.bin` files on a spare device before production deployment
- ⚠️ **Pico W:** `.bin` file MUST be compiled with same Flash Size settings (include FS partition)
- ⚠️ **ESP32:** `.bin` file must match your board type (ESP32, ESP32-S2, ESP32-C3, etc.)

---

## �🛠️ Using in Your Own Projects (Pico W first, ESP32 optional)

```cpp
#include <pico_ota.h>

const char *ssid = "YourWiFi";
const char *password = "YourPassword";

void setup() {
  Serial.begin(115200);
  
  // Start OTA with optional hostname and password
  otaSetup(ssid, password, "my-device", "my-ota-password");
  
  // Your setup code here...
}

void loop() {
  otaLoop();  // Must call this frequently to handle OTA requests
  
  // Your loop code here...
}
```

**API Reference (same on both platforms):**

**Basic Setup:**
- `otaSetup(ssid, password, hostname, otaPassword)` - Initialize OTA (call once in `setup()`, uses default timeout and auto-format)
- `otaLoop()` - Handle OTA requests (call frequently in `loop()`)

**Configuration (call before otaSetup):**
- `otaSetWifiTimeout(timeoutMs)` - Set Wi-Fi connection timeout (default: 30000ms)
- `otaSetFsAutoFormat(enabled)` - Enable/disable filesystem auto-format on Pico W (default: true)

**Callbacks (call before otaSetup):**
- `otaOnStart(callback)` - Called when OTA update starts
- `otaOnProgress(callback)` - Called during OTA update with progress (current, total bytes)
- `otaOnEnd(callback)` - Called when OTA update completes successfully
- `otaOnError(callback)` - Called when OTA update fails with error code

**Advanced Setup:**
- `otaSetupWithTimeout(ssid, password, timeoutMs, hostname, otaPassword, allowFsFormat)` - Full control over timeout and FS behavior (returns bool success)

**Status Queries:**
- `otaIsConnected()` - Returns true if Wi-Fi is connected
---

## 🎯 Production-Ready Features (v1.3.0+)

For classroom and field deployments, use these reliability features:

### Non-Blocking Wi-Fi with Timeout

**Problem:** Original code blocks forever if Wi-Fi unavailable  
**Solution:** Configure timeout before calling `otaSetup()`

```cpp
otaSetWifiTimeout(15000);  // 15 second timeout
bool success = otaSetupWithTimeout(ssid, password, 15000, hostname, otaPassword, false);
if (!success) {
  Serial.println("OTA failed to start - continuing in offline mode");
  // Your fallback logic here
}
```

### OTA Callbacks for User Feedback

**Problem:** No feedback during OTA updates  
**Solution:** Register callbacks before calling `otaSetup()`

```cpp
void onOtaStart() {
  Serial.println("OTA started - do not power off!");
  digitalWrite(LED_PIN, HIGH);
}

void onOtaProgress(unsigned int current, unsigned int total) {
  Serial.printf("Progress: %u%%\n", (current * 100) / total);
}

void setup() {
  otaOnStart(onOtaStart);
  otaOnProgress(onOtaProgress);
  otaOnEnd([]() { Serial.println("OTA complete!"); });
  otaOnError([](int err) { Serial.printf("OTA error: %d\n", err); });
  
  otaSetup(ssid, password, hostname, otaPassword);
}
```

### Filesystem Safety (Pico W)

**Problem:** Auto-format on mount failure causes data loss  
**Solution:** Disable auto-format for production

```cpp
otaSetFsAutoFormat(false);  // Prevents accidental data loss
otaSetup(ssid, password, hostname, otaPassword);
```

### Status Monitoring

```cpp
void loop() {
  if (otaIsConnected() && otaIsReady()) {
    otaLoop();  // Only handle OTA when ready
  }
  
  // Your application code
  if (otaIsConnected()) {
    // Network-dependent features
  }
}
```

**Complete Example:** See `examples/Non_Blocking_OTA/` for production-ready patterns

---

## 🌐 HTTP Pull-Based OTA (v1.4.0+)

Download firmware updates from an HTTP server. Perfect for production deployments where devices pull updates from a central server.

```cpp
#include <pico_ota.h>

void setup() {
  otaSetup(ssid, password, hostname, otaPassword);
}

void loop() {
  otaLoop();
  
  // Check for updates periodically
  if (shouldCheckUpdate()) {
    int result = otaUpdateFromUrl("http://your-server.com/firmware.bin", "1.0.0");
    
    if (result == OTA_UPDATE_OK) {
      // Device will reboot automatically
    } else if (result == OTA_UPDATE_NO_UPDATE) {
      Serial.println("Already running latest version");
    }
  }
}
```

**API Functions:**
- `otaUpdateFromUrl(url)` - Download and install firmware from URL
- `otaUpdateFromUrl(url, currentVersion)` - With version checking (server can respond 304 Not Modified)
- `otaUpdateFromHost(host, port, path)` - Download from host:port/path
- `otaUpdateFromHost(host, port, path, currentVersion)` - With version checking

**Return Codes:**
| Code | Constant | Meaning |
|------|----------|---------|
| 0 | `OTA_UPDATE_OK` | Update successful (device will reboot) |
| 1 | `OTA_UPDATE_NO_UPDATE` | Already running latest version |
| -1 | `OTA_UPDATE_FAILED` | Download or install failed |
| -2 | `OTA_UPDATE_NO_WIFI` | No WiFi connection |
| -3 | `OTA_UPDATE_HTTP_ERROR` | HTTP request failed |

**Complete Example:** See `examples/HTTP_Pull_OTA/`

---

## 🌍 Web Browser Upload (v1.4.0+)

Start a web server that allows uploading firmware directly from any web browser. Great for non-technical users!

```cpp
#include <pico_ota.h>

void setup() {
  otaSetup(ssid, password, hostname, otaPassword);
  
  // Optional: Enable authentication
  otaSetWebCredentials("admin", "secret123");
  
  // Start web server on port 80
  otaStartWebServer(80);
  
  Serial.print("Upload firmware at: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/update");
}

void loop() {
  otaLoop();  // Handles both ArduinoOTA and web server
}
```

**Usage:**
1. Open a web browser
2. Navigate to `http://<device-ip>/update`
3. Select your `.bin` firmware file
4. Click "Update" and wait for completion

**API Functions:**
- `otaStartWebServer(port)` - Start web server (default port 80)
- `otaStopWebServer()` - Stop web server
- `otaSetWebCredentials(username, password)` - Enable HTTP authentication
- `otaIsWebServerRunning()` - Check if web server is active

**Complete Example:** See `examples/WebBrowser_OTA/`

---

## 📦 GitHub Release Auto-Update (v1.4.0+)

Automatically check GitHub Releases for new firmware versions and update when available. Perfect for open-source projects!

```cpp
#include <pico_ota.h>

const char* CURRENT_VERSION = "1.0.0";

void setup() {
  // Configure GitHub repository
  otaSetGitHubRepo("username", "my-project");
  otaSetCurrentVersion(CURRENT_VERSION);
  otaSetGitHubAssetName("*.bin");  // Match any .bin file
  
  otaSetup(ssid, password, hostname, otaPassword);
}

void loop() {
  otaLoop();
  
  // Check for updates (e.g., hourly)
  if (shouldCheckUpdate()) {
    char latestVersion[32];
    int result = otaCheckGitHubUpdate(latestVersion, sizeof(latestVersion));
    
    if (result == OTA_UPDATE_OK) {
      Serial.printf("New version available: %s\n", latestVersion);
      otaUpdateFromGitHub();  // Download and install
    }
  }
}
```

**GitHub Setup:**
1. Create a repository for your project
2. Create a Release (e.g., tag `v1.1.0`)
3. Attach your compiled `.bin` firmware file to the release
4. Configure the repo details in your sketch

**API Functions:**
- `otaSetGitHubRepo(owner, repo)` - Set GitHub owner/repo (e.g., "username", "my-project")
- `otaSetCurrentVersion(version)` - Set current firmware version for comparison
- `otaSetGitHubAssetName(pattern)` - Asset filename pattern (`*.bin`, `firmware-pico.bin`, etc.)
- `otaCheckGitHubUpdate(latestVersion, maxLen)` - Check for new release
- `otaUpdateFromGitHub()` - Download and install latest release
- `otaGetLatestGitHubVersion()` - Get latest version string

**Return Codes:**
| Code | Constant | Meaning |
|------|----------|---------|
| 0 | `OTA_UPDATE_OK` | Update available (or successful) |
| 1 | `OTA_UPDATE_NO_UPDATE` | Already running latest version |
| -4 | `OTA_UPDATE_PARSE_ERROR` | Failed to parse GitHub API response |
| -5 | `OTA_UPDATE_NO_ASSET` | No matching firmware asset in release |

**Complete Example:** See `examples/GitHub_OTA/`

---

## 🔄 WiFi Auto-Reconnect (v1.4.0+)

Automatically reconnect to WiFi if the connection drops. Essential for reliable IoT deployments.

```cpp
#include <pico_ota.h>

void onWifiLost() {
  Serial.println("WiFi disconnected!");
  // Maybe blink an LED or pause network-dependent tasks
}

void onWifiRestored() {
  Serial.println("WiFi reconnected!");
  // Resume normal operation
}

void setup() {
  // Enable auto-reconnect
  otaSetAutoReconnect(true);
  otaSetReconnectInterval(30000);    // Try every 30 seconds
  otaSetMaxReconnectAttempts(10);    // Give up after 10 attempts (0 = infinite)
  
  // Optional: Get notified of connection changes
  otaOnWifiDisconnect(onWifiLost);
  otaOnWifiReconnect(onWifiRestored);
  
  otaSetup(ssid, password, hostname, otaPassword);
}

void loop() {
  otaLoop();  // Handles auto-reconnect automatically
}
```

**API Functions:**
- `otaSetAutoReconnect(enabled)` - Enable/disable auto-reconnect
- `otaSetReconnectInterval(ms)` - Time between reconnect attempts (default: 30000ms)
- `otaSetMaxReconnectAttempts(count)` - Max attempts before giving up (0 = infinite)
- `otaOnWifiDisconnect(callback)` - Called when WiFi connection is lost
- `otaOnWifiReconnect(callback)` - Called when WiFi is restored

---

## 🔧 Troubleshooting

| Problem | Solution |
|---------|----------|
| ❌ `ERR: No Filesystem` (Pico W) | Re-select Flash Size with FS partition (e.g., "2MB Sketch + 1MB FS") and re-upload via USB |
| ❌ Device hangs at "Connecting WiFi..." | Use `otaSetWifiTimeout(15000)` and `otaSetupWithTimeout()` for graceful timeout |
| ❌ Filesystem data lost after OTA | Call `otaSetFsAutoFormat(false)` before `otaSetup()` to prevent auto-format |

### ESP32 note and example reference

ESP32 users can use the same API (`otaSetup`/`otaLoop`) and start from the minimal example at:

- `examples/ESP32_OTA_test/ESP32_OTA_test.ino`
- `examples/ESP32_OTA_test/secret.h`

The example defaults to GPIO 2 for LED feedback (common on many ESP32 dev boards). Adjust the pin if your board differs.

| Problem | Solution |
|---------|----------|
| ❌ Device doesn't appear in Network Ports | Check Serial Monitor for IP address, ensure same Wi-Fi network, check firewall settings |
| ❌ OTA upload fails | Verify OTA password matches, check device is powered and connected to Wi-Fi |
| ❌ LittleFS mount failed | Flash may be corrupt - sketch will auto-format on first run (wait ~30 seconds) |

---

## 📁 Repository Structure

```
Pico_OTA/
├─ 📄 library.properties      
├─ 📂 src/
│  ├─ pico_ota.h              
│  └─ pico_ota.cpp            
├─ 📂 examples/
│  ├─ 📂 Pico_OTA_test/              (Basic single-core example)
│  │  ├─ Pico_OTA_test.ino    
│  │  └─ secret.h
│  ├─ 📂 Pico_OTA_test_with_Dual_Core/  (Dual-core for RP2040)
│  │  ├─ Pico_OTA_test_with_Dual_Core.ino    
│  │  └─ secret.h
│  ├─ 📂 ESP32_OTA_test/             (ESP32 example)
│  │  ├─ ESP32_OTA_test.ino
│  │  └─ secret.h
│  ├─ 📂 Non_Blocking_OTA/           (Production-ready patterns)
│  │  ├─ Non_Blocking_OTA.ino
│  │  └─ secret.h
│  ├─ 📂 HTTP_Pull_OTA/              (Pull firmware from server)
│  │  ├─ HTTP_Pull_OTA.ino
│  │  └─ secret.h
│  ├─ 📂 WebBrowser_OTA/             (Browser-based upload)
│  │  ├─ WebBrowser_OTA.ino
│  │  └─ secret.h
│  └─ 📂 GitHub_OTA/                 (GitHub release auto-update)
│     ├─ GitHub_OTA.ino
│     └─ secret.h
├─ 📄 README.md                
└─ 📄 LICENSE                
```

---

## 🔐 Security Notes

- 🔒 **Use strong OTA passwords** in production (not "admin"!)
- 🚫 **Never commit credentials** - use `secret.h` and add to `.gitignore`
- 🌐 **OTA only works on local network** (same LAN as your computer)
- 🔑 Consider implementing additional authentication for production use

---

## 📜 License

MIT License - See [LICENSE](LICENSE) for details.

---

## 🤝 Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

---

## 📚 Learn More

- [Arduino-Pico Documentation](https://arduino-pico.readthedocs.io/)
- [Raspberry Pi Pico W Datasheet](https://datasheets.raspberrypi.com/picow/pico-w-datasheet.pdf)

---

<div align="center">

**Made with ❤️ for the Raspberry Pi Pico W community**

⭐ Star this repo if you find it useful!

</div>
