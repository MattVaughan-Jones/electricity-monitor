#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_status_line(int status_code) {
  switch (status_code) {
  case 200:
    return "200 OK";
  case 400:
    return "400 Bad Request";
  case 500:
    return "500 Internal Server Error";
  default:
    return "500 Internal Server Error";
  }
}

char *build_req(char *response, int status_code) {
  char *template = "HTTP/1.1 %s\r\n"
                   "Content-Type: text/plain\r\n"
                   "Connection: close\r\n"
                   "Content-Length: %d\r\n\r\n"
                   "%s";

  char *status_line = get_status_line(status_code);

  int response_len = strlen(response);

  int req_len =
      snprintf(NULL, 0, template, status_line, response_len, response) + 1;
  char *req = malloc(req_len);
  if (req == NULL)
    return NULL;

  snprintf(req, req_len, template, status_line, response_len, response);

  return req;
}

char *ws_connection_req(char *domain, char *PORT, char *ws_req_key) {
  char *template = "GET /websocket-connect HTTP/1.1\r\n"
                   "Host: %s:%s\r\n"
                   "Upgrade: websocket\r\n"
                   "Connection: Upgrade\r\n"
                   "Sec-WebSocket-Key: %s\r\n"
                   "Sec-WebSocket-Version: 13\r\n\r\n";

  int req_len = snprintf(NULL, 0, template, domain, PORT, ws_req_key) + 1;

  char *req = malloc(req_len);
  if (!req)
    return NULL;

  snprintf(req, req_len, template, domain, PORT, ws_req_key);

  return req;
}

char *ws_connection_upgrade_res(char *ws_accept_key) {
  // char *outcome = "HTTP/1.1 101 Switching Protocols\r\n"
  //                 "Upgrade: websocket\r\n"
  //                 "Connection: Upgrade\r\n"
  //                 "Sec-WebSocket-Accept:
  //                 s3pPLMBiTxaQ9kYGzzhZRbK+xOo=\r\n\r\n";

  char *template = "HTTP/1.1 101 Switching Protocols\r\n"
                   "Upgrade: websocket\r\n"
                   "Connection: Upgrade\r\n"
                   "Sec-WebSocket-Accept: %s\r\n\r\n";

  int req_len = snprintf(NULL, 0, template, ws_accept_key) + 1;

  char *req = malloc(req_len);
  if (!req)
    return NULL;

  snprintf(req, req_len, template, ws_accept_key);

  return req;
}
