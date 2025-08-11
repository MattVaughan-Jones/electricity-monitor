#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *build_req(char *response, const int len) {
  char *template = "HTTP/1.1 200 OK\r\n"
                   "Content-Type: text/plain\r\n"
                   "Connection: close\r\n"
                   "Content-Length: %d\r\n\r\n"
                   "%s";

  int req_len = snprintf(NULL, 0, template, len, response) + 1;
  char *req = malloc(req_len);
  if (req == NULL)
    return NULL;

  snprintf(req, req_len, template, len, response);

  return req;
}

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
