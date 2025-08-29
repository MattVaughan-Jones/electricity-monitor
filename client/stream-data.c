#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ws-protocol/ws-encode.h"

void stream_data(int sock_fd) {
  char string1[10] = "Hey there";
  // char string2[128] = "This will be a 128 byte string:
  // lwsiafjliwjelwijclweijflisejfaliwjfalfjailjflaisjefdlasjefoajwijlwajijasilfjfijwalefjaseijfeals";

  unsigned char *ws_frame_buf = ws_encode(string1, 1);
  printf("frame:\n");
  print_binary_bytes(ws_frame_buf, 15);

  send(sock_fd, ws_frame_buf, sizeof(ws_frame_buf), 0);
  // sleep(2);
  // send(sock_fd, ws_encoded2, sizeof(ws_encoded2), 0);
}