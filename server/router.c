#include "controllers/websocket_connect.h"
#include "http-util.h"
#include "http/http.h"
#include "controllers/device_control.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define MAX_RECEIVE_SIZE 1024

// Helper function to safely compare paths with bounds checking
static int path_matches(const char* path, const char* expected) {
    if (!path || !expected) {
        return 0;
    }
    
    if (strlen(path) != strlen(expected)) {
        return 0;
    }
    
    return strcmp(path, expected) == 0;
}

static int get_path_and_method(char *recv_buf, char *path, char *method) {
  if (!recv_buf || !path || !method) {
    fprintf(stderr, "undefined argument passed to get_path\n");
    return 1;
  }
  char version[20], first_line[256];

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
  char method[10];
  char recv_buf[MAX_RECEIVE_SIZE];

  // receive the incoming message
  int num_bytes;
  if ((num_bytes = recv(client_fd, recv_buf, MAX_RECEIVE_SIZE, 0)) == -1) {
    perror("recv");
    send_response(client_fd, "Internal server error: failed to receive data", 500);
    return;
  }
  if (num_bytes >= MAX_RECEIVE_SIZE) {
    send_response(client_fd, "Request entity too large", 413);
    return;
  }
  if (num_bytes == 0) {
    // client disconnected
    return;
  }
  recv_buf[num_bytes] = '\0';

  // route on URL path
  if (get_path_and_method(recv_buf, path, method) != 0) {
    send_response(client_fd, "Internal server error: unable to parse path", 500);
    return;
  }
  // TODO - implement standard 405 responses below
  if (path_matches(path, "/websocket-connect")) {
    if (strncmp(method, "GET", strlen("GET")) == 0) {
      controller_websocket_connect(recv_buf, client_fd);
    }
    else {
      send_405_response(client_fd);
    }
  } else if (path_matches(path, "/start-recording")) {
    if (strncmp(method, "POST", strlen("POST")) == 0) {
      controller_start_recording(client_fd);
    }
    else {
      send_405_response(client_fd);
    }
  } else if (path_matches(path, "/stop-recording")) {
    if (strncmp(method, "GET", strlen("GET")) == 0) {
      controller_stop_recording(client_fd);
    }
    else {
      send_405_response(client_fd);
    }
  } else {
    send_404_response(client_fd);
  }
}
