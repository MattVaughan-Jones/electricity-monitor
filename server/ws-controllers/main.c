#include "ws.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

void signal_start_recording(int client_fd) {
  char *start_recording = "{\"action\":\"start_recording\"}";
  ws_frame_buf_t *ws_frame_buf = ws_encode(start_recording, 1);
  if (!ws_frame_buf->frame_buf) {
    fprintf(stderr, "failed to allocate frame_buf");
    return;
  }
  if (!ws_frame_buf->len) {
    fprintf(stderr, "failed to allocate len");
    return;
  }

  printf("sending start_recording signal\n");

  send(client_fd, ws_frame_buf->frame_buf, ws_frame_buf->len, 0);
  
  free(ws_frame_buf->frame_buf);
  free(ws_frame_buf);
}

void handle_websocket_communication(int client_fd) {
  if (client_fd <= 0) {
    fprintf(stderr, "Websocket controller error: Invalid client_fd");
    return;
  }
  unsigned char buffer[1024];

  // Make the device start recording data
  signal_start_recording(client_fd);

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
