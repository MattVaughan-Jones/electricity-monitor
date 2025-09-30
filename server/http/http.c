#include "http-util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void send_response(const int client_fd, const char *message, const int status) {
  char *msg = build_req((char *)message, status);
  if (msg == NULL) {
    fprintf(stderr, "failed to build composable response message");
    return;
  }

  int msg_len = strlen(msg);
  if (send(client_fd, msg, msg_len, 0) == -1) {
    perror("failed to send composable response");
  }
  free(msg);
}

void send_404_response(int client_fd) {
  send_response(client_fd, "Not found", 404);
}

void send_405_response(int client_fd) {
  send_response(client_fd, "Method not allowed", 405);
}
