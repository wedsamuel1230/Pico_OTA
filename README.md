# PICO_OTA — Arduino OTA helper for Raspberry Pi Pico W

> Arduino library that brings over‑the‑air (OTA) updates to the Raspberry Pi Pico W using the Arduino-Pico core. Ships with a ready-to-run example that connects to Wi‑Fi, starts ArduinoOTA, and blinks the onboard LED so you can verify the loop stays responsive.

## Repository layout
```
Pico_OTA_test/
├─ library.properties   
├─ src/
│  ├─ ota.h
│  └─ ota.cpp
├─ examples/
│  └─ Pico_OTA_test/
│    └─ Pico_OTA_test.ino   
└─ README.md
```

## Requirements
- Board: Raspberry Pi Pico W (Arduino-Pico core by Earle Philhower)
- Flash layout: choose a scheme with LittleFS (e.g., 2MB Flash / 1MB FS) so OTA can stage updates.
- Network: device and host on the same LAN for OTA uploads.

## Quickstart
1. Install the Raspberry Pi Pico/RP2040 core (Arduino-Pico) in Arduino IDE.
2. Open `File → Examples → PICO_OTA → Pico_OTA_test`.
3. replace `Your_SSID` and `Your_PASSWORD` in `examples/Pico_OTA_test/Pico_OTA_test.ino` with your network credentials.
4. (Optional) Set an OTA password by passing a 4th argument to `otaSetup`, e.g. `otaSetup(ssid, password, "pico-host", "strong-ota-pass");`.
5. Select the Pico W board, correct port, and a Flash Size that includes LittleFS.
6. Upload once over USB. After reboot, note the IP printed on Serial, then upload OTA via the network port.

## Usage notes
- Call `otaSetup(ssid, password, hostname, otaPassword)` once; call `otaLoop()` frequently in `loop()`.
- Keep real credentials out of source control (use the provided `secret.h` which is gitignored, or your own provisioning).
- If OTA fails with `ERR: No Filesystem`, reflash with a Flash Size that includes LittleFS to allow formatting.

## License
MIT License. See `LICENSE` for details.
