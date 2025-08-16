#include <stddef.h>
#include <stdio.h>
#include <string.h>

int validate_sock_con_req(char *recv_buf) {
  if (recv_buf == NULL)
    return -1;

  if (strstr(recv_buf, "GET /websocket-connect HTTP/1.1") == NULL) {
    fprintf(stderr, "Server: invalid websocket connection request: %s",
            recv_buf);
    return -2;
  }
  if (strstr(recv_buf, "Upgrade: websocket") == NULL) {
    fprintf(stderr, "Server: invalid websocket connection request: %s",
            recv_buf);
    return -2;
  }
  if (strstr(recv_buf, "Sec-WebSocket-Key:") == NULL) {
    fprintf(stderr,
            "Server: invalid websocket connection request (missing "
            "sec-websocket-key):\n%s",
            recv_buf);
    return -3;
  }
  if (strstr(recv_buf, "Sec-WebSocket-Version: 13") == NULL) {
    fprintf(stderr, "Server: invalid websocket connection request: %s",
            recv_buf);
    return -4;
  }

  return 0;
}