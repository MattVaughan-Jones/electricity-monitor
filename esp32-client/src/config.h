#ifndef CONFIG_H
#define CONFIG_H

// WiFi Configuration
#define WIFI_SSID "Cable-879a-2G"
#define WIFI_PASSWORD "EarnsWavy289"

// Server Configuration
#define SERVER_HOST "192.168.0.134"  // Update to your server's IP address
#define SERVER_PORT 8080
#define SERVER_PATH "/websocket-connect"

// Hardware Configuration
#define LED_PIN 2
#define STATUS_LED_PIN 2

// Data Streaming Configuration
#define DATA_INTERVAL_MS 500  // Send data every 2 seconds
#define RECONNECT_INTERVAL_MS 5000

// Debug Configuration
#define DEBUG_SERIAL true
#define DEBUG_LEVEL 3

#endif
