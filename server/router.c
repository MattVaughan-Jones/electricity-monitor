#include "http.h"
#include "validators.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define MAX_RECEIVE_SIZE 1024

int get_path(char *recv_buf, char *path) {
  char method[10], version[20];

  char *newline = strchr(recv_buf, '\n');
  if (!newline)
    return 1;

  size_t first_line_len = newline - recv_buf;
  char first_line[256];
  strncpy(first_line, recv_buf, first_line_len);
  first_line[first_line_len] = '\0';

  sscanf(first_line, "%s %s %s", method, path, version);
  return 0;
}

void router(int client_fd) {
  char path[256];
  char recv_buf[MAX_RECEIVE_SIZE];

  // receive the incoming message
  int num_bytes;
  if ((num_bytes = recv(client_fd, recv_buf, MAX_RECEIVE_SIZE, 0)) == -1) {
    perror("recv");
    exit(1);
  }

  recv_buf[num_bytes] = '\0';
  printf("received from client:\n%s", recv_buf);

  int get_path_result;
  if ((get_path_result = get_path(recv_buf, path)) != 0) {
    // TODO - return a 5xx response here
    // TODO - probably modify build_req() to accept a status code as an arg
    char *response = "Internal server error: unable to parse path\n";
    char *msg = build_req(response);

    int message_len = strlen(msg);
    send(client_fd, msg, message_len, 0);
  }

  if (strncmp(path, "/websocket-connect", strlen(path)) == 0) {
    int isValid = validate_sock_con_req(recv_buf);
    char *response;
    if (isValid != 0) {
      response = "Error: invalid socket connection request\n";
    } else {
      response = "initiating websocket handshake\n";
    }
    char *msg = build_req(response);

    int message_len = strlen(msg);
    send(client_fd, msg, message_len, 0);
  } else {
    char *response = "Server says hello :)\n";
    char *msg = build_req(response);

    int message_len = strlen(msg);
    send(client_fd, msg, message_len, 0);
  }
}
