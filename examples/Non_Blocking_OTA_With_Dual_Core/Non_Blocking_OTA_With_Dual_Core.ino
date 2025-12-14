/*━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * Raspberry Pi Pico W - Dual-Core Non-Blocking OTA Example
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * WHAT THIS DEMONSTRATES:
 * ✓ Core 0: Main application (LED status, monitoring) - never blocks!
 * ✓ Core 1: Production-ready OTA server with advanced features
 * ✓ Non-blocking WiFi connection (15s timeout)
 * ✓ OTA callbacks for progress feedback
 * ✓ Filesystem safety (no auto-format)
 * ✓ WiFi auto-reconnect (v1.4.0)
 * ✓ Visual status via LED patterns
 * ✓ Graceful offline fallback
 * 
 * WHY DUAL-CORE?
 * • Core 0 runs your application WITHOUT BLOCKING for OTA operations
 * • Core 1 handles all OTA/WiFi independently with dedicated resources
 * • Perfect for real-time tasks: sensors, motors, timers, displays
 * • Production-ready: non-blocking, callbacks, safety features
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * ARDUINO IDE SETUP (REQUIRED BEFORE UPLOADING):
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * STEP 1: Select Board
 *   Tools → Board → Raspberry Pi RP2040 Boards → "Raspberry Pi Pico W"
 * 
 * STEP 2: Configure Flash Size ⚠️ CRITICAL!
 *   Tools → Flash Size → "2MB (Sketch: 1MB, FS: 1MB)"
 *   ⚠️  DO NOT select "2MB (No FS)" - OTA needs filesystem!
 * 
 * STEP 3: Edit secret.h
 *   Open secret.h in this folder and configure:
 *   • ssid = "Your_SSID"
 *   • password = "Your_PASSWORD"
 *   • hostname = "pico-ota-dual"
 *   • otaPassword = "admin" (change for security!)
 * 
 * STEP 4: First Upload (USB Required)
 *   • Connect Pico W via USB
 *   • Tools → Port → Select COM port (Windows) or /dev/ttyACM0 (Linux/Mac)
 *   • Click Upload
 *   • Open Serial Monitor (115200 baud)
 *   • Watch LED status and note IP address
 * 
 * STEP 5: Wireless OTA Uploads (After First Upload)
 *   • Tools → Port → Network Ports → Select "pico-ota-dual at <IP>"
 *   • Click Upload - wireless! 🎉
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * LED STATUS PATTERNS:
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * • SLOW BLINK (1s):  Core 1 connecting to WiFi...
 * • SOLID ON:         OTA ready! Can accept wireless uploads
 * • FAST BLINK (250ms): OTA failed (check WiFi credentials)
 * • OFF:              Not initialized yet
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * CORE ARCHITECTURE:
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * 
 * CORE 0 (This file):
 *   • setup()  - Initialize LED, serial, application
 *   • loop()   - Application logic, LED control, status monitoring
 *   • Reads: otaReady, blinkPattern (set by Core 1)
 *   • Never calls OTA functions directly
 * 
 * CORE 1 (secret.h):
 *   • setup1() - Initialize OTA with production features
 *   • loop1()  - Handle OTA requests continuously
 *   • Sets: otaReady, blinkPattern (read by Core 0)
 *   • Manages all WiFi/OTA operations
 * 
 *━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━*/

#include <Arduino.h>
#include "secret.h"  // Contains WiFi credentials + Core 1 OTA code (setup1/loop1)

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Core 0 Configuration
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

// LED Pin (adjust for your board if needed)
#if defined(LED_BUILTIN)
  const int LED_PIN = LED_BUILTIN;
#else
  const int LED_PIN = 25;  // Pico W default
#endif

// Shared state variables (written by Core 1, read by Core 0)
extern bool otaReady;       // Set by Core 1 when OTA is initialized
extern int blinkPattern;    // Set by Core 1: 0=off, 1=slow, 2=solid, 3=fast

// Local state for LED timing
unsigned long lastBlink = 0;

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Core 0 Setup - Application Initialization
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void setup() {
  // Initialize serial (Core 1 will use this too)
  Serial.begin(115200);
  delay(1000);  // Brief delay for serial stability
  
  // Initialize LED for status indication
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Print startup banner
  Serial.println("\n\n╔════════════════════════════════════════════════════════╗");
  Serial.println("║   Pico W Dual-Core Non-Blocking OTA Example           ║");
  Serial.println("╚════════════════════════════════════════════════════════╝");
  Serial.println();
  Serial.println("[Core 0] Main application initializing...");
  Serial.println("[Core 0] LED status indicator: Ready");
  Serial.println("[Core 0] Core 1 will handle OTA independently");
  Serial.println();
  
  // Note: Core 1 (setup1 in secret.h) runs in parallel
  // It will initialize OTA and set otaReady + blinkPattern
  
  Serial.println("[Core 0] Setup complete. Entering main loop...\n");
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Core 0 Main Loop - Application Logic
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void loop() {
  // Update LED based on OTA status (set by Core 1)
  updateLED();
  
  // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  // YOUR APPLICATION CODE GOES HERE
  // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  // This runs on Core 0 and NEVER BLOCKS for OTA operations!
  // Perfect for:
  //   • Reading sensors
  //   • Controlling motors/actuators
  //   • Processing data in real-time
  //   • Updating displays
  //   • Running timers/state machines
  // 
  // Core 1 handles all WiFi/OTA operations independently
  // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  
  // Example: Print status every 10 seconds
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 10000) {
    lastStatus = millis();
    Serial.printf("[Core 0] Application running... (OTA %s)\n",
                  otaReady ? "Ready" : "Not Ready");
  }
  
  // Small delay for responsiveness
  delay(100);
}

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// LED Status Indicator Helper Function
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
void updateLED() {
  unsigned long now = millis();
  
  switch (blinkPattern) {
    case 0:  // Off - OTA not initialized yet
      digitalWrite(LED_PIN, LOW);
      break;
      
    case 1:  // Slow blink (1s period) - Core 1 connecting to WiFi
      if (now - lastBlink > 1000) {
        lastBlink = now;
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      }
      break;
      
    case 2:  // Solid ON - OTA ready for wireless uploads!
      digitalWrite(LED_PIN, HIGH);
      break;
      
    case 3:  // Fast blink (250ms) - OTA failed (check WiFi credentials)
      if (now - lastBlink > 250) {
        lastBlink = now;
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      }
      break;
  }
}
