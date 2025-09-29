#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "config.h"
#include "ws.h"
#include "wifi_manager.h"
#include "websocket_client.h"

// Global variables
WiFiClient wifiClient;  // Make this global!
bool recording = false;

// Function declarations
void sendElectricityData();
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


void sendElectricityData() {
  if (wifiClient.connected()) {
    // Create JSON payload with electricity data
    StaticJsonDocument<300> doc;
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
  } else if (action == "stop_recording") {
    recording = false;
    Serial.println("Stopped recording");
  }
}
