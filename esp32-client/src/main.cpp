#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <ModbusMaster.h>
#include "config.h"
#include "ws.h"
#include "wifi_manager.h"
#include "websocket_client.h"

// Global variables
WiFiClient wifiClient;  // Make this global!
bool recording = false;
unsigned long recordingStartTime = 0;  // Timestamp when recording started

// Modbus setup
ModbusMaster node;
const uint8_t MODBUS_SLAVE_ID = 1;  // JSY-MK-194G default slave ID
const uint32_t MODBUS_BAUD_RATE = 4800;  // Working baud rate

// Function declarations
void sendElectricityData();
void handleIncomingMessage(String message);

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Electricity Monitor Client Starting...");
  
  // Initialize LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Initialize Modbus communication
  Serial2.begin(MODBUS_BAUD_RATE, SERIAL_8N1, 17, 16);  // RX=GPIO17, TX=GPIO16
  node.begin(MODBUS_SLAVE_ID, Serial2);
  
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
    doc["type"] = "data";
    doc["timestamp"] = millis() - recordingStartTime;  // Relative time in milliseconds
    
    // Read actual data from JSY-MK-194G via Modbus
    uint8_t result;
    
    // Read JSY-MK-194G electrical parameters (4-byte registers)
    result = node.readHoldingRegisters(0x0048, 2);  // Voltage (4 bytes)
    if (result == node.ku8MBSuccess) {
      uint32_t voltage_raw = ((uint32_t)node.getResponseBuffer(0) << 16) | node.getResponseBuffer(1);
      doc["voltage"] = voltage_raw / 10000.0;
    } else {
      return;
    }
    
    result = node.readHoldingRegisters(0x0049, 2);  // Current (4 bytes)
    if (result == node.ku8MBSuccess) {
      uint32_t current_raw = ((uint32_t)node.getResponseBuffer(0) << 16) | node.getResponseBuffer(1);
      doc["current"] = current_raw / 10000.0;
    } else {
      return;
    }
    
    result = node.readHoldingRegisters(0x004A, 2);  // Power (4 bytes)
    if (result == node.ku8MBSuccess) {
      uint32_t power_raw = ((uint32_t)node.getResponseBuffer(0) << 16) | node.getResponseBuffer(1);
      doc["power"] = power_raw / 10000.0;
    } else {
      return;
    }
    
    result = node.readHoldingRegisters(0x004F, 2);  // Frequency (4 bytes)
    if (result == node.ku8MBSuccess) {
      uint32_t freq_raw = ((uint32_t)node.getResponseBuffer(0) << 16) | node.getResponseBuffer(1);
      doc["frequency"] = freq_raw / 100.0;
    } else {
      doc["frequency"] = 0;
    }
    
    // Convert to string and send
    String jsonString;
    serializeJson(doc, jsonString);
    
    sendWebSocketFrame(wifiClient, jsonString);
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
    recordingStartTime = millis();  // Record the start time
  } else if (action == "stop_recording") {
    recording = false;
    recordingStartTime = 0;  // Reset start time
  }
}
