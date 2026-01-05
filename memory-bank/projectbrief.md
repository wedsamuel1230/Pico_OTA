# Project Brief — PICO_OTA

**Project Type:** Arduino Library  
**Tech Stack:** C++17, Arduino IDE, Arduino-Pico Core, ArduinoOTA  
**Primary Language:** C++  
**Target Platforms:** Raspberry Pi Pico W, Pico 2 W, ESP32  
**License:** MIT (with LGPL v2.1 dependencies)

---

## Goals

1. Provide Over-The-Air (OTA) firmware update capability for Raspberry Pi Pico W/2W
2. Support multiple OTA methods: ArduinoOTA, HTTP Pull, GitHub releases, Web Browser
3. Maintain optional ESP32 compatibility with unified API
4. Ensure ease of use for Arduino community (library installable via Library Manager)

---

## Constraints

- Must work within Arduino IDE ecosystem and conventions
- Must respect Arduino Library Specification (library.properties format)
- Must handle filesystem requirements (LittleFS for Pico W)
- Must maintain backward compatibility with existing user sketches
- Copyright holder: Samuel F. (full name: Samuel Fong)
- GitHub repo: https://github.com/wedsamuel1230/PICO_OTA

---

## Current Version

**v1.4.1** — Documentation release (December 14, 2025)

---

## Release Management

- Version format: MAJOR.MINOR.PATCH (semantic versioning)
- Release notes maintained in `releasenote.md`
- Copyright year updated annually
- All releases require:
  - Version bump in `library.properties`
  - Updated `releasenote.md` entry
  - Copyright year verification in LICENSE and source files
  - Git tag with version number
