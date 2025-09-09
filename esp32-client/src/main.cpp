#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "config.h"

// Global variables
AsyncWebSocketClient* wsClient = nullptr;
WiFiClient client;  // Make this global!
unsigned long lastDataTime = 0;
bool connected = false;

// Function declarations
void connectToWiFi();
void connectToWebSocket();
void sendElectricityData();
void sendWebSocketFrame(WiFiClient& client, const String& message);
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

// Function to generate a random WebSocket mask
void generateRandomMask(uint8_t* mask) {
  // Use ESP32's hardware random number generator
  for (int i = 0; i < 4; i++) {
    mask[i] = random(256); // Generate random byte (0-255)
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Electricity Monitor Client Starting...");
  
  // Initialize LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Connect to WiFi
  connectToWiFi();
  
  // Connect to WebSocket
  connectToWebSocket();
  
  Serial.println("Setup complete!");
}

void loop() {
  // Send data at regular intervals
  if (connected && millis() - lastDataTime > DATA_INTERVAL_MS) {
    sendElectricityData();
    lastDataTime = millis();
  }
  
  delay(100);
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Blink LED while connecting
  }
  
  digitalWrite(LED_PIN, HIGH); // Solid LED when connected
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void connectToWebSocket() {
  // Note: ESPAsyncWebServer is primarily a server library
  // For client functionality, we might need to use a different approach
  // Let me create a simple WebSocket client using raw TCP
  
  if (client.connect(SERVER_HOST, SERVER_PORT)) {
    Serial.println("TCP connected, initiating WebSocket handshake...");
    
    // Generate WebSocket key
    String wsKey = "dGhlIHNhbXBsZSBub25jZQ=="; // Base64 encoded key
    
    // Send WebSocket upgrade request
    String request = "GET " + String(SERVER_PATH) + " HTTP/1.1\r\n";
    request += "Host: " + String(SERVER_HOST) + ":" + String(SERVER_PORT) + "\r\n";
    request += "Upgrade: websocket\r\n";
    request += "Connection: Upgrade\r\n";
    request += "Sec-WebSocket-Key: " + wsKey + "\r\n";
    request += "Sec-WebSocket-Version: 13\r\n";
    request += "\r\n";
    
    client.print(request);
    
    // Read response
    String response = "";
    unsigned long timeout = millis() + 5000;
    while (millis() < timeout && client.connected()) {
      if (client.available()) {
        char c = client.read();
        response += c;
        if (response.indexOf("\r\n\r\n") != -1) {
          break;
        }
      }
    }
    
    // Check if upgrade was successful
    if (response.indexOf("101 Switching Protocols") != -1) {
      connected = true;
      digitalWrite(LED_PIN, HIGH);
      Serial.println("WebSocket connection established!");
    } else {
      Serial.println("WebSocket handshake failed");
      Serial.println("Response: " + response);
      client.stop();
    }
  } else {
    Serial.println("Failed to connect to server");
  }
}

void sendWebSocketFrame(WiFiClient& client, const String& message) {
  if (connected && client.connected()) {
    int payload_len = message.length();
    
    // Check if payload is too large for 16-bit length
    if (payload_len > 65535) {
      Serial.printf("ERROR: Message too large (%d bytes), maximum supported is 65535 bytes. Skipping message.\n", payload_len);
      return; // Skip sending this message
    }
    
    int frame_len;
    
    // Calculate frame length based on payload size
    if (payload_len <= 125) {
      frame_len = 2 + 4 + payload_len; // header + mask + payload
    } else {
      frame_len = 2 + 2 + 4 + payload_len; // header + extended length + mask + payload
    }
    
    uint8_t frame[frame_len];
    int pos = 0;
    
    // Header
    frame[pos++] = 0x81; // FIN + text frame
    
    if (payload_len <= 125) {
      frame[pos++] = 0x80 | payload_len; // MASK bit + payload length
    } else {
      frame[pos++] = 0x80 | 126; // MASK bit + extended length indicator
      frame[pos++] = (payload_len >> 8) & 0xFF; // High byte
      frame[pos++] = payload_len & 0xFF; // Low byte
    }
    
    // Generate a random mask
    uint8_t mask[4];
    generateRandomMask(mask);
    frame[pos++] = mask[0];
    frame[pos++] = mask[1];
    frame[pos++] = mask[2];
    frame[pos++] = mask[3];
    
    // Copy and mask the message data
    for (int i = 0; i < message.length(); i++) {
      frame[pos++] = message[i] ^ mask[i % 4];
    }
    
    Serial.printf("DEBUG: Sending frame of %d bytes (payload: %d): ", frame_len, payload_len);
    for (int i = 0; i < frame_len && i < 20; i++) {
      Serial.printf("%02x ", frame[i]);
    }
    Serial.println();
    
    // Also print the message being sent
    Serial.printf("DEBUG: Message: %s\n", message.c_str());
    
    client.write(frame, frame_len);
    Serial.printf("=== NEXT IS A NEW FRAME ===\n\n");
  }
}

void sendElectricityData() {
  if (connected) {
    // Create JSON payload with electricity data
    StaticJsonDocument<300> doc;
    doc["device_id"] = "esp32_client";
    doc["timestamp"] = millis();
    doc["data"] = "55555"; // Your original test data
    
    // Simulate sensor readings (replace with actual sensor code)
    doc["voltage"] = random(220, 240);
    doc["current"] = random(5, 15);
    doc["power"] = doc["voltage"].as<float>() * doc["current"].as<float>();
    doc["frequency"] = 50.0;
    
    // Add metadata
    doc["uptime"] = millis();
    doc["free_heap"] = ESP.getFreeHeap();
    doc["wifi_rssi"] = WiFi.RSSI();
    
    // Convert to string and send
    String jsonString;
    serializeJson(doc, jsonString);
    
    Serial.printf("Sending data: %s\n", jsonString.c_str());
    sendWebSocketFrame(client, jsonString);  // ← Add this line!
    
    // Blink LED to indicate data transmission
    digitalWrite(LED_PIN, LOW);
    delay(50);
    digitalWrite(LED_PIN, HIGH);
  } else {
    Serial.println("WebSocket not connected, skipping data transmission");
  }
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  // This is for server-side WebSocket handling
  // Not needed for client implementation
}