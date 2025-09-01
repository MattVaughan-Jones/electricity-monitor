#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ws-protocol/ws.h"

int stream_data(int sock_fd) {
  char string1[6] = "55555\0";
  // char string2[130] = "This will be a 128 byte "
  //                     "string:"
  //                     "lwsiafjliwjelwijclweijflisejfaliwjfalfjailjflaisjefdlasj"
  //                     "efoajwijlwajijasilfjfijwalefjaseijfealsss";

  ws_frame_buf_t *ws_frame_buf = ws_encode(string1, 1);
  if (!ws_frame_buf->frame_buf) {
    fprintf(stderr, "failed to allocate frame_buf");
    return -1;
  }
  if (!ws_frame_buf->len) {
    fprintf(stderr, "failed to allocate len");
    return -1;
  }

  printf("sending message: %s\n", string1);

  send(sock_fd, ws_frame_buf->frame_buf, ws_frame_buf->len, 0);

  sleep(1);
  send(sock_fd, ws_frame_buf->frame_buf, ws_frame_buf->len, 0);

  free(ws_frame_buf->frame_buf);
  free(ws_frame_buf);
  return 0;
}