#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include <WiFi.h>

// Function declarations
void connectToWebSocket();
void sendWebSocketFrame(WiFiClient& wifiClient, const String& message);
String readWebSocketMessage();

#endif
