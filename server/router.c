#include "http_req.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void router(int client_fd) {
  char recv_buf[1024];
  char method[10], path[256], version[20];

  // receive the incoming message
  int num_bytes;
  if ((num_bytes = recv(client_fd, recv_buf, 1024, 0)) == -1) {
    perror("recv");
    exit(1);
  }

  recv_buf[num_bytes] = '\0';
  printf("received from client:\n%s", recv_buf);

  char *first_line = strtok(recv_buf, "\n");
  sscanf(first_line, "%s %s %s", method, path, version);

  if (strncmp(path, "/websocket-connect", strlen(path)) == 0) {
    char *response = "initiating websocket handshake\n";
    int response_len = strlen(response);
    char *msg = build_headers(response, response_len);

    int message_len = strlen(msg);
    send(client_fd, msg, message_len, 0);
  } else {
    char *response = "Server says hello :)\n";
    int response_len = strlen(response);
    char *msg = build_headers(response, response_len);

    int message_len = strlen(msg);
    send(client_fd, msg, message_len, 0);
  }
}
