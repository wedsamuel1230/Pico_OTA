# Release Notes — PICO_OTA v1.2.0

**Date:** December 13, 2025  
**Type:** Patch/Minor Feature (ESP32 optional support, Pico W focus)

---

## 🎯 Summary

PICO_OTA v1.2.0 maintains primary focus on **Raspberry Pi Pico W** while introducing optional **ESP32** compatibility. The API (`otaSetup`, `otaLoop`) remains unchanged. Pico W continues to require a Flash Size with a **LittleFS** partition; ESP32 OTA works without a filesystem partition.

---

## ✨ What's Changed

### **1. ESP32 Platform Support (optional)** 🧩

- ✅ `src/pico_ota.h`: Broadened board guard to include `ARDUINO_ARCH_ESP32`.
- ✅ `src/pico_ota.cpp`: Conditional LittleFS logic (required only on Pico W); ESP32 OTA path skips filesystem setup.
- ✅ `library.properties`: `architectures=rp2040,esp32`, sentence/paragraph updated; version remains **1.2.0**.
- ✅ `README.md`: Updated to document ESP32 usage and per-target board settings.

### **2. New Example: `examples/ESP32_OTA_test`** 🧪

- Minimal sketch using the same API (`otaSetup`/`otaLoop`).
- Includes a `secret.h` placeholder (SSID, password, hostname, OTA password).
- Optional LED blink to visualize OTA-deployed changes.

---

## 🎓 Notes for Users (Pico W first)

- On **Pico W**: select a Flash Size that includes **LittleFS** (e.g., Sketch: 1MB, FS: 1MB). This is mandatory for OTA staging.
- On **ESP32**: default Flash settings are fine; OTA does not require a filesystem partition.
- Keep credentials in `secret.h`; do not commit secrets.

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

