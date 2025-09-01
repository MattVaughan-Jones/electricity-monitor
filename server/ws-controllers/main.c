#include "ws.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

void handle_websocket_communication(int client_fd) {
  if (client_fd <= 0) {
    fprintf(stderr, "Websocket controller error: Invalid client_fd");
    return;
  }
  unsigned char buffer[1024];

  while (1) {
    int bytes = recv(client_fd, buffer, sizeof(buffer), 0);

    if (bytes == 0) {
      break;
    }
    if (bytes < 0) {
      perror("recv");
      break;
    }

    char *message = ws_decode(buffer, bytes);
    if (!message) {
      fprintf(stderr, "Websocket controller error: Unable to decode buffer");
      break;
    }
    printf("received message: %s\n", message);
    free(message);
  }

  close(client_fd);
}
