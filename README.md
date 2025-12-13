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

## 🛠️ Using in Your Own Projects (Pico W first, ESP32 optional)

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
- `otaSetup(ssid, password, hostname, otaPassword)` - Initialize OTA (call once in `setup()`)
  - `hostname` and `otaPassword` are optional (pass `nullptr` to skip)
- `otaLoop()` - Handle OTA requests (call frequently in `loop()`)

---

## 🔧 Troubleshooting

| Problem | Solution |
|---------|----------|
| ❌ `ERR: No Filesystem` (Pico W) | Re-select Flash Size with FS partition (e.g., "2MB Sketch + 1MB FS") and re-upload via USB |

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
│  └─ 📂 Pico_OTA_test/
│  │  ├─ Pico_OTA_test.ino    
│  │  └─ secret.h
│  ├─ 📂 Pico_OTA_test_with_Dual_Core/
│  │  ├─ Pico_OTA_test_with_Dual_Core.ino    
│  │  └─ secret.h                  
│  └─ 📂 ESP32_OTA_test/
│     ├─ ESP32_OTA_test.ino
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
