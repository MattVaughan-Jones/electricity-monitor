#include "websocket_client.h"
#include "config.h"
#include "ws.h"

#include <WiFi.h>

extern WiFiClient wifiClient; // Reference to global wifiClient from main.cpp

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
