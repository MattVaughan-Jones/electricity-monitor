#include <stddef.h>
#include <stdio.h>
#include <string.h>

int validate_sock_con_req(char *recv_buf) {
  if (recv_buf == NULL) {
    fprintf(stderr, "Server: NULL buffer passed to validate_sock_con_req");
    return -1;
  }

  size_t buf_len = strlen(recv_buf);
  if (buf_len > 8192) {
    fprintf(stderr, "Server: request buffer too large (%zu bytes)\n", buf_len);
    return -1;
  }

  if (strstr(recv_buf, "GET /websocket-connect HTTP/1.1") == NULL) {
    fprintf(stderr, "Server: invalid websocket connection request: %s\n",
            recv_buf);
    return -1;
  }
  if (strstr(recv_buf, "Upgrade: websocket") == NULL) {
    fprintf(stderr, "Server: invalid websocket connection request: %s\n",
            recv_buf);
    return -1;
  }
  if (strstr(recv_buf, "Sec-WebSocket-Key:") == NULL) {
    fprintf(stderr,
            "Server: invalid websocket connection request (missing "
            "sec-websocket-key): %s\n",
            recv_buf);
    return -1;
  }
  if (strstr(recv_buf, "Sec-WebSocket-Version: 13") == NULL) {
    fprintf(stderr, "Server: invalid websocket connection request: %s\n",
            recv_buf);
    return -1;
  }

  return 0;
}