#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "config.h"
#include "ws.h"

// Global variables
WiFiClient wifiClient;  // Make this global!
bool recording = false;

// Function declarations
void connectToWiFi();
void connectToWebSocket();
void sendElectricityData();
void sendWebSocketFrame(WiFiClient& wifiClient, const String& message);
String readWebSocketMessage();
void handleIncomingMessage(String message);

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Electricity Monitor Client Starting...");
  
  // Initialize LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  connectToWiFi();
  
  connectToWebSocket();
  
  Serial.println("Setup complete!");
}

void loop() {
  if (!wifiClient.connected()) {
    printf("WiFi client disconnected!\n");
    return;
  }
  
  // Check for incoming WebSocket messages from server
  if (wifiClient.available()) {
    String message = readWebSocketMessage();
    if (message.length() > 0) {
      Serial.printf("Message from server: %s\n", message.c_str());
      handleIncomingMessage(message);
    }
  }
  
  // Only send data if recording (less frequent)
  static unsigned long lastDataTime = 0;
  if (wifiClient.connected() && recording && (millis() - lastDataTime >= DATA_INTERVAL_MS)) {
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
  
  digitalWrite(LED_PIN, LOW); // LED off when connected
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void connectToWebSocket() {
  if (wifiClient.connect(SERVER_HOST, SERVER_PORT)) {
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
    
    Serial.println("Sending ws upgrade req to server");
    wifiClient.print(request); // Send to server
    
    // Read response
    String response = "";
    unsigned long timeout = millis() + 5000;
    while (millis() < timeout && wifiClient.connected()) {
      if (wifiClient.available()) {
        char c = wifiClient.read();
        response += c;
        if (response.indexOf("\r\n\r\n") != -1) {
          break;
        }
      }
    }
    
    // Check if upgrade was successful
    // Don't bother checking the Sec-Websocket-Accept value
    if (response.indexOf("101 Switching Protocols") != -1) {
      Serial.println("WebSocket connection established!");
    } else {
      Serial.println("WebSocket handshake failed");
      Serial.println("Response: " + response);
      wifiClient.stop();
    }
  } else {
    Serial.println("Failed to connect to server");
  }
}

void sendWebSocketFrame(WiFiClient& wifiClient, const String& message) {
  if (wifiClient.connected()) {
    ws_frame_buf_t *frame = ws_encode(message.c_str(), 1);
    
    if (frame && frame->frame_buf) {
      Serial.printf("Sending data: %s\n", message.c_str());
      wifiClient.write(frame->frame_buf, frame->len);
      
      free(frame->frame_buf);
      free(frame);
    } else {
      Serial.println("Failed to encode WebSocket frame");
    }
  }
}

void sendElectricityData() {
  if (wifiClient.connected()) {
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
    
    sendWebSocketFrame(wifiClient, jsonString);
  } else {
    Serial.println("WebSocket not connected, skipping data transmission");
  }
}

String readWebSocketMessage() {
  uint8_t buffer[1024];
  int bytes = wifiClient.read(buffer, sizeof(buffer));
  
  if (bytes <= 0) {
    return "";
  }
  
  char* message = ws_decode(buffer, bytes);
  
  if (!message) {
    Serial.println("Failed to decode WebSocket message from server");
    return "";
  }
  
  String result = String(message);
  free(message);
  
  return result;
}

void handleIncomingMessage(String message) {
  StaticJsonDocument<200> doc;
  deserializeJson(doc, message);

  String action = doc["action"];
  String type = doc["type"];
  
  // Handle ping/pong messages
  if (type == "ping") {
    sendWebSocketFrame(wifiClient, "{\"type\":\"pong\"}");
    return;
  }
  
  // Handle action messages
  if (action == "start_recording") {
    recording = true;
    Serial.println("Started recording");
  } else if (action == "stop_recording") {
    recording = false;
    Serial.println("Stopped recording");
  }
}
