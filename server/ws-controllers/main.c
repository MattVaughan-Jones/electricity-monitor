#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

void handle_websocket_communication(int client_fd) {
  char buffer[1024];

  while (1) {
    int bytes = 0;
    bytes = recv(client_fd, buffer, sizeof(buffer), 0);

    if (bytes == 0) {
      break;
    }
    if (bytes < 0) {
      perror("recv");
      break;
    }

    buffer[bytes + 1] = '\0';

    printf("Received ws frame:\n");
    for (int i = 0; i < bytes; i++) {
      for (int bit = 7; bit >= 0; bit--) {
        printf("%d", ((unsigned char)buffer[i] >> bit) & 1);
      }
      printf("\n");
    }
    printf("\n");
  }

  close(client_fd);
}
