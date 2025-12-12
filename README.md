# 🚀 PICO_OTA — Wireless Arduino Updates for Raspberry Pi Pico W

<div align="center">

**Upload code wirelessly to your Raspberry Pi Pico W - no USB cable needed!**

[![Arduino](https://img.shields.io/badge/Arduino-Compatible-00979D?style=flat&logo=arduino)](https://www.arduino.cc/)
[![Platform](https://img.shields.io/badge/Platform-RP2040-C51A4A?style=flat)](https://www.raspberrypi.com/documentation/microcontrollers/rp2040.html)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

</div>

---

## 📖 What is This?

A simple Arduino library that enables **Over-The-Air (OTA)** updates for Raspberry Pi Pico W using the Arduino-Pico core.

**How it works:**
1. 📡 Upload your sketch once via USB - it connects to Wi-Fi and starts OTA server
2. ✨ After that, upload wirelessly from Arduino IDE - no more plugging/unplugging!
3. 💡 Test it by uncommenting LED blink code and uploading via OTA

---

## 📋 What You Need

| Requirement | Details |
|------------|---------|
| 🎛️ **Board** | Raspberry Pi Pico W or Pico 2 W |
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

### Step 2️⃣: Configure Board Settings ⚠️ CRITICAL

| Setting | Value | Why? |
|---------|-------|------|
| **Board** | `Tools → Board → Raspberry Pi RP2040 Boards →`<br>`Raspberry Pi Pico W` (or Pico 2 W) | Selects your hardware |
| **Flash Size** | `Tools → Flash Size →`<br>`2MB (Sketch: 1MB, FS: 1MB)` ✅<br>or `16MB (Sketch: 8MB, FS: 8MB)` ✅ | OTA needs filesystem space to stage updates |
| **Port** | `Tools → Port →`<br>Windows: `COMx` (e.g., COM3)<br>Mac/Linux: `/dev/ttyACM0` or `/dev/cu.usbmodem*` | For USB upload |

> ⚠️ **DO NOT select "2MB (No FS)"** - OTA will fail without filesystem space!

---

### Step 3️⃣: Configure Wi-Fi Credentials

1. Open example: **`File → Examples → PICO_OTA → Pico_OTA_test`**
2. Edit these lines in the sketch:
   ```cpp
   const char *ssid = "Your_SSID";        // ← Your Wi-Fi network name
   const char *password = "Your_PASSWORD"; // ← Your Wi-Fi password
   ```
3. (Optional) Customize settings:
   ```cpp
   const char *hostname = "pico-ota";     // ← Device name (shows in network ports)
   const char *otaPassword = "admin";      // ← OTA upload password (or set to nullptr)
   ```

> 💡 **Pro tip:** Create a `secret.h` file for credentials to keep them out of git!

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

```cpp
// setup() runs on Core 0
void setup() {
  Serial.begin(115200);
  // Your setup code
}

// setup1() runs on Core 1
void setup1() {
  otaSetup(ssid, password, "my-device", "password");
}

// loop() runs on Core 0 - your main application
void loop() {
  // Your responsive application code here
  // Never blocks for OTA updates!
  delay(100);
}

// loop1() runs on Core 1 - OTA server
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

Want to verify OTA is really working? Here's a simple test:

1. ✅ Upload the example via USB (LED code is commented out by default)
2. ✏️ In the sketch, **uncomment** these lines:
   ```cpp
   // Uncomment these lines:
   const int ledPin = LED_BUILTIN;
   unsigned long lastBlink = 0;
   const unsigned long blinkIntervalMs = 500;
   
   // ... and in setup():
   pinMode(ledPin, OUTPUT);
   digitalWrite(ledPin, LOW);
   
   // ... and in loop():
   const unsigned long now = millis();
   if (now - lastBlink >= blinkIntervalMs) {
     lastBlink = now;
     digitalWrite(ledPin, !digitalRead(ledPin));
   }
   ```
3. 📡 Upload again via **OTA (wireless port)**
4. 💡 The LED starts blinking - **OTA works!**

---

## 🛠️ Using in Your Own Projects

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

**API Reference:**
- `otaSetup(ssid, password, hostname, otaPassword)` - Initialize OTA (call once in `setup()`)
  - `hostname` and `otaPassword` are optional (pass `nullptr` to skip)
- `otaLoop()` - Handle OTA requests (call frequently in `loop()`)

---

## 🔧 Troubleshooting

| Problem | Solution |
|---------|----------|
| ❌ `ERR: No Filesystem` | Re-select Flash Size with FS partition (e.g., "2MB Sketch + 1MB FS") and re-upload via USB |
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
│     ├─ Pico_OTA_test.ino    
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
