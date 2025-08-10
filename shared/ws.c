#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *ws_connection_req(char *domain, char *PORT, char *key) {
  char *template = "GET /websocket-connect HTTP/1.1\r\n"
                   "Host: %s:%s\r\n"
                   "Upgrade: websocket\r\n"
                   "Connection: Upgrade\r\n"
                   "Sec-WebSocket-Key: %s\r\n"
                   "Sec-WebSocket-Version: 13\r\n\r\n";

  int req_len = snprintf(NULL, 0, template, domain, PORT, key) + 1;

  char *req = malloc(req_len);
  if (!req)
    return NULL;

  snprintf(req, req_len, template, domain, PORT, key);

  return req;
}
