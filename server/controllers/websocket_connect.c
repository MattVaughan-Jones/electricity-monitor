#include "../validators.h"
#include "../ws-controllers/main.h"
#include "http.h"
#include "ws-http-keys.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

static int get_swk_from_header(char *swk_encoded, char *recv_buf) {
  if (!swk_encoded || !recv_buf) {
    fprintf(stderr, "undefined argument passed to get_swk_from_header\n");
    return 1;
  }

  int result = 1;
  char *req = malloc(strlen(recv_buf) + 1);
  if (!req) {
    fprintf(stderr,
            "failed to allocate memory for req in get_swk_from_header\n");
    return 1;
  }

  strcpy(req, recv_buf);
  char *key_line = strstr(req, "Sec-WebSocket-Key:");

  if (key_line) {
    char *key_start = key_line + strlen("Sec-WebSocket-Key:");
    while (*key_start == ' ')
      key_start++;

    char *key_end = strstr(key_line, "\r\n");
    if (key_end) {
      int swk_encoded_len = key_end - key_start;
      if (swk_encoded_len == SEC_WEBSOCKET_KEY_SIZE) {
        strncpy(swk_encoded, key_start, swk_encoded_len);
        swk_encoded[swk_encoded_len] = '\0';
        result = 0;
      }
    }
  }
  free(req);
  return result;
}

static void send_invalid_req_response(int client_fd) {
  char *msg = build_req("Invalid web socket connection request\n", 400);
  if (msg == NULL) {
    fprintf(stderr, "failed to allocate msg in controller_websocket_connect "
                    "for failed validation\n");
    return;
  }

  int message_len = strlen(msg);
  if (send(client_fd, msg, message_len, 0) == -1) {
    perror("send");
    return;
  }
  free(msg);
}

static void send_unable_to_find_swk(int client_fd) {
  char *response = "Internal server error: unable to find Sec-Websocket-Key";
  char *msg = build_req(response, 500);
  if (msg == NULL) {
    fprintf(stderr, "failed to allocate msg in "
                    "controller_websocket_connect for parsing header\n");
    return;
  }

  int message_len = strlen(msg);
  if (send(client_fd, msg, message_len, 0) == -1) {
    perror("send");
    free(msg);
    return;
  }
  free(msg);
}

void controller_websocket_connect(char *recv_buf, int client_fd) {
  if (!recv_buf || !client_fd) {
    fprintf(stderr,
            "undefined argument passed to controller_websocket_connect\n");
    return;
  }

  if (validate_sock_con_req(recv_buf) != 0) {
    send_invalid_req_response(client_fd);
    return;
  }

  char *swk_encoded = malloc(SEC_WEBSOCKET_KEY_SIZE + 1);
  if (swk_encoded == NULL) {
    fprintf(stderr,
            "failed to allocate swk_encoded in controller_websocket_connect\n");
    return;
  }

  if (get_swk_from_header(swk_encoded, recv_buf) != 0) {
    send_unable_to_find_swk(client_fd);
    free(swk_encoded);
    return;
  }

  char *sec_websocket_accept_key = generate_swa(swk_encoded);
  if (sec_websocket_accept_key == NULL) {
    fprintf(stderr, "failed to allocate sec_websocket_accept_key\n");
    free(swk_encoded);
    return;
  }

  free(swk_encoded);

  char *res = ws_connection_upgrade_res(sec_websocket_accept_key);
  if (res == NULL) {
    fprintf(stderr, "failed to allocate res in controller_websocket_connect\n");
    free(sec_websocket_accept_key);
    return;
  }

  int res_len = strlen(res);
  if (send(client_fd, res, res_len, 0) == -1) {
    perror("send");
    free(sec_websocket_accept_key);
    free(res);
    return;
  }

  handle_websocket_communication(client_fd);

  free(sec_websocket_accept_key);
  free(res);
}
