#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "config.h"
#include "ws.h"

// Global variables
WiFiClient client;  // Make this global!
bool connected = false;
bool recording = false;

// Function declarations
void connectToWiFi();
void connectToWebSocket();
void sendElectricityData();
void sendWebSocketFrame(WiFiClient& client, const String& message);
String readWebSocketMessage();
void handleServerCommand(String command);

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
  // Check for incoming WebSocket messages from server
  if (client.available()) {
    String message = readWebSocketMessage();
    if (message.length() > 0) {
      handleServerCommand(message);
    }
  }
  
  // Only send data if recording
  if (connected && recording) {
    sendElectricityData();
  }
  
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
    ws_frame_buf_t *frame = ws_encode(message.c_str(), 1);
    
    if (frame && frame->frame_buf) {
      Serial.printf("Sending data: %s\n", message.c_str());
      client.write(frame->frame_buf, frame->len);
      
      free(frame->frame_buf);
      free(frame);
    } else {
      Serial.println("Failed to encode WebSocket frame");
    }
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

String readWebSocketMessage() {
  if (!client.connected() || !client.available()) {
    return "";
  }
  
  // Read raw WebSocket frame data
  uint8_t buffer[1024];
  int bytes = client.read(buffer, sizeof(buffer));
  
  if (bytes <= 0) {
    return "";
  }
  
  Serial.printf("Received %d bytes from server\n", bytes);
  
  // Decode the WebSocket frame using shared library
  char* message = ws_decode(buffer, bytes);
  
  if (!message) {
    Serial.println("Failed to decode WebSocket message from server");
    return "";
  }
  
  String result = String(message);
  free(message); // Important: free the memory allocated by ws_decode
  
  Serial.println("Decoded message: " + result);
  return result;
}

void handleServerCommand(String command) {
  StaticJsonDocument<200> doc;
  deserializeJson(doc, command);
  
  String action = doc["action"];
  
  if (action == "start_recording") {
    recording = true;
    Serial.println("Started recording");
  } else if (action == "stop_recording") {
    recording = false;
    Serial.println("Stopped recording");
  }
}
