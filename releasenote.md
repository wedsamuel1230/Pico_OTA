# Release Notes — PICO_OTA v1.1.1

**Date:** December 13, 2025  
**Type:** Patch Release (Documentation & Code Organization)

---

## 🎯 Summary

PICO_OTA v1.1.1 improves the **dual-core example** by moving OTA setup to `secret.h`, simplifying the main sketch for students and keeping configuration separate from application code.

---

## ✨ What's Changed

### **1. Simplified Dual-Core Sketch** 🧹

**Before:** Students had to understand dual-core functions scattered in main sketch  
**After:** Main sketch (`Pico_OTA_test_with_Dual_Core.ino`) contains only `setup()` and `loop()`

- ✅ Moved `setup1()` and `loop1()` (OTA server) to `secret.h`
- ✅ Moved Wi-Fi credentials to `secret.h`
- ✅ Main sketch now 100% student code area
- ✅ OTA infrastructure is automatic and encapsulated

### **2. Better Code Organization** 📁

**secret.h now contains:**
```cpp
#include <Arduino.h>
#include <pico_ota.h>

// Wi-Fi & OTA credentials (EDIT THESE)
const char *ssid = "Your_SSID";
const char *password = "Your_PASSWORD";
const char *hostname = "pico-ota";
const char *otaPassword = "admin";

// OTA Server on Core 1 (AUTOMATIC - don't modify)
void setup1() { /* OTA initialization */ }
void loop1() { /* OTA handler */ }
```

**Main sketch contains:**
```cpp
#include "secret.h"  // All config and OTA setup automatic

void setup() {
  // YOUR application setup code
}

void loop() {
  // YOUR main application code (never blocks for OTA!)
}
```

### **3. Updated Documentation** 📖

**README.md changes:**
- Step 3: Clarified that Wi-Fi credentials go in `secret.h` only
- Dual-Core Section: Explained the separation of concerns
- LED Testing: Simplified to match new structure

**Sketch comments:**
- Clarified that OTA setup is in `secret.h`
- Updated configuration instructions
- Simplified LED test workflow

---

## 🎓 For Students

✅ **What to edit:**
- Only `secret.h` — your Wi-Fi credentials and OTA password

✅ **What NOT to edit:**
- Main sketch structure
- `setup1()` and `loop1()` functions
- OTA API calls

✅ **Your code:**
- Write your application in `setup()` and `loop()`
- OTA updates work in the background on Core 1

---

## 📋 Files Modified

| File | Change |
|------|--------|
| `examples/Pico_OTA_test_with_Dual_Core/Pico_OTA_test_with_Dual_Core.ino` | Removed OTA setup, updated comments |
| `examples/Pico_OTA_test_with_Dual_Core/secret.h` | Added `setup1()`, `loop1()`, credentials |
| `README.md` | Updated Step 3, dual-core section, LED testing section |

---

## ✅ Benefits

- 🎯 **Clearer Learning:** Students see their code area vs. infrastructure
- 🔒 **Safer:** OTA setup is hidden, can't accidentally break it
- 📝 **Better Separation:** Configuration (secret.h) vs. Code (main sketch)
- 🚀 **Faster Setup:** No need to understand dual-core details to start coding

---

## 🔄 Backward Compatibility

✅ **Fully compatible** with v1.1.0
- Single-core example unchanged
- Library API unchanged
- No breaking changes

---

## 📦 How to Update

Simply pull the latest code:
```bash
git pull origin main
```

No changes needed to existing projects using v1.1.0.

---

## 🙏 Notes

This is a **patch release** focused on improving code clarity and student experience. The functionality remains identical to v1.1.0.

