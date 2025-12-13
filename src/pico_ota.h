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

// Optional: provide otaPassword to require authentication for uploads.
void otaSetup(const char *ssid,
			  const char *password,
			  const char *hostname = nullptr,
			  const char *otaPassword = nullptr);
void otaLoop();
