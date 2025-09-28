#include "ws.h"
#include "../ipc.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <time.h>
#include <string.h>

void handle_websocket_communication(int client_fd) {
  if (client_fd <= 0) {
    fprintf(stderr, "Websocket controller error: Invalid client_fd");
    return;
  }
  
  unsigned char buffer[1024];
  while (1) {
    int bytes = recv(client_fd, buffer, sizeof(buffer), 0);

    if (bytes == 0) {
      printf("ESP32 disconnected (connection closed)\n");
      *ws_fd = -1;  // Reset shared WebSocket file descriptor
      break;
    }
    
    if (bytes < 0) {
      // Check if it's a temporary error (like EAGAIN/EWOULDBLOCK)
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        usleep(10000);
        continue;
      } else {
        // Real error - ESP32 disconnected
        printf("ESP32 disconnected (error: %s)\n", strerror(errno));
        *ws_fd = -1;  // Reset shared WebSocket file descriptor
        break;
      }
    }

    char *message = ws_decode(buffer, bytes);
    if (!message) {
      fprintf(stderr, "Websocket controller error: Unable to decode buffer\n");
      continue;
    }
    
    // TODO - store the message somewhere
    printf("received message: %s\n", message);
    free(message);
  }

  close(client_fd);
  exit(0);  // Exit the forked process
}
