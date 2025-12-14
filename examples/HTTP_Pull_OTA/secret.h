// WiFi credentials
#define WIFI_SSID     "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"


// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Configuration
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

// Firmware URL - change this to your server
const char* FIRMWARE_URL = "http://your-server.com/firmware.bin";

// Current firmware version (for version checking)
const char* CURRENT_VERSION = "1.0.0";

// Check for updates every 5 minutes
const unsigned long CHECK_INTERVAL_MS = 5 * 60 * 1000;