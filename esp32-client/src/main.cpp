#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include "config.h"

// Global variables
WiFiClient client;  // Make this global!
bool connected = false;

// Function declarations
void connectToWiFi();
void connectToWebSocket();
void sendElectricityData();
void sendWebSocketFrame(WiFiClient& client, const String& message);

// Function to generate a random WebSocket mask
void generateRandomMask(uint8_t* mask) {
  for (int i = 0; i < 4; i++) {
    mask[i] = random(256);
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
  if (connected) {
    sendElectricityData();
  }
  
  // Send data at regular intervals
  delay(DATA_INTERVAL_MS);
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
  
  digitalWrite(LED_PIN, LOW); // LED off when connected
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void connectToWebSocket() {
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
    
    client.print(request); // Send to server
    
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
    // Don't bother checking the Sec-Websocket-Accept value
    if (response.indexOf("101 Switching Protocols") != -1) {
      connected = true;
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
      return;
    }
    
    int frame_len;
    
    // Calculate frame length
    if (payload_len <= 125) {
      frame_len = 2 + 4 + payload_len; // header + mask + payload
    } else {
      frame_len = 2 + 2 + 4 + payload_len; // header + extended length + mask + payload
    }
    
    uint8_t frame[frame_len];
    int frame_position = 0;
    
    // Header
    frame[frame_position++] = 0x81; // FIN + text frame
    
    if (payload_len <= 125) {
      frame[frame_position++] = 0x80 | payload_len; // MASK bit + payload length
    } else {
      frame[frame_position++] = 0x80 | 126; // MASK bit + extended length indicator
      frame[frame_position++] = (payload_len >> 8) & 0xFF; // High byte
      frame[frame_position++] = payload_len & 0xFF; // Low byte
    }
    
    // Generate a random mask
    uint8_t mask[4];
    generateRandomMask(mask);

    // Set mask
    frame[frame_position++] = mask[0];
    frame[frame_position++] = mask[1];
    frame[frame_position++] = mask[2];
    frame[frame_position++] = mask[3];
    
    // Mask and set the message
    for (int i = 0; i < message.length(); i++) {
      frame[frame_position++] = message[i] ^ mask[i % 4];
    }
    
    Serial.printf("Sending data: %s\n", message.c_str());
    
    client.write(frame, frame_len);
  }
}

void sendElectricityData() {
  if (connected) {
    // Create JSON payload with electricity data
    StaticJsonDocument<300> doc;
    doc["device_id"] = "esp32_client";
    doc["timestamp"] = millis();
    
    // Simulate sensor readings (replace with actual sensor code)
    doc["voltage"] = random(220, 240);
    doc["current"] = random(5, 15);
    doc["power"] = doc["voltage"].as<float>() * doc["current"].as<float>();
    doc["frequency"] = 50.0;
    
    // Convert to string and send
    String jsonString;
    serializeJson(doc, jsonString);
    
    sendWebSocketFrame(client, jsonString);
  } else {
    Serial.println("WebSocket not connected, skipping data transmission");
  }
}
