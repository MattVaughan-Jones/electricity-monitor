#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void controller_default(int client_fd) {
  const char *response = "Server says hello :)\n";
  char *msg = build_req((char *)response, 200);
  if (msg == NULL) {
    fprintf(stderr, "failed to build default response message");
    return;
  }

  int message_len = strlen(msg);
  if (send(client_fd, msg, message_len, 0) == -1) {
    perror("failed to send default response");
  }
  free(msg);
}