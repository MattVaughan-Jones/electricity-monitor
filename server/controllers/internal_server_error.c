#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void internal_server_error(int client_fd, char *input) {
  char *template = "Internal server error: %s\n";
  int response_len = snprintf(NULL, 0, template, input);

  char *response = malloc(response_len + 1);
  if (!response) {
    fprintf(stderr, "failed to allocate response for internal server error\n");
    return;
  }

  snprintf(response, response_len, template, input);

  char *msg = build_req(response, 500);
  if (msg == NULL) {
    fprintf(stderr, "failed to allocate msg for internal server error\n");
    free(response);
    return;
  }

  int message_len = strlen(msg);
  if (send(client_fd, msg, message_len, 0) == -1) {
    perror("send");
  }
  free(msg);
  free(response);
}
