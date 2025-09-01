#include "controllers/default.h"
#include "controllers/internal_server_error.h"
#include "controllers/websocket_connect.h"
#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define MAX_RECEIVE_SIZE 1024

static int get_path(char *recv_buf, char *path) {
  if (!recv_buf || !path) {
    fprintf(stderr, "undefined argument passed to get_path\n");
    return 1;
  }
  char method[10], version[20], first_line[256];

  char *newline = strchr(recv_buf, '\n');
  if (!newline)
    return 1;

  size_t first_line_len = newline - recv_buf;
  if (first_line_len > sizeof(first_line)) {
    fprintf(stderr, "First line of HTTP request too long to parse. Max size is "
                    "255 characters\n");
    return 1;
  }

  strncpy(first_line, recv_buf, first_line_len);
  first_line[first_line_len] = '\0';

  if (sscanf(first_line, "%s %s %s", method, path, version) != 3) {
    fprintf(stderr, "Failed to parse first line of HTTP request\n");
    return 1;
  }

  return 0;
}

void router(int client_fd) {
  if (client_fd < 0) {
    fprintf(stderr, "Router error: Invalid client_fd\n");
    return;
  }

  char path[256];
  char recv_buf[MAX_RECEIVE_SIZE];

  // receive the incoming message
  int num_bytes;
  if ((num_bytes = recv(client_fd, recv_buf, MAX_RECEIVE_SIZE, 0)) == -1) {
    perror("recv");
    internal_server_error(client_fd, "failed to receive data");
    return;
  }
  if (num_bytes >= MAX_RECEIVE_SIZE) {
    char *msg = build_req("Request entity too large", 413);
    if (msg == NULL) {
      fprintf(stderr, "Router error: unable to allocate msg for 'request too "
                      "large' response\n");
      return;
    }
    if (send(client_fd, msg, strlen(msg), 0) == -1) {
      perror("send");
      free(msg);
      return;
    }
    free(msg);
    return;
  }
  if (num_bytes == 0) {
    // client disconnected
    return;
  }
  recv_buf[num_bytes] = '\0';

  // route on URL path
  if (get_path(recv_buf, path) != 0) {
    internal_server_error(client_fd, "unable to parse path");
    return;
  }
  if (strncmp(path, "/websocket-connect", strlen("/websocket-connect")) == 0) {
    controller_websocket_connect(recv_buf, client_fd);
  } else {
    controller_default(client_fd);
  }
}
