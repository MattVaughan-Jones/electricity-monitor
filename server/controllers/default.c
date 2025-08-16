#include "http.h"

#include <string.h>
#include <sys/socket.h>

void controller_default(int client_fd) {
  char *response = "Server says hello :)\n";
  char *msg = build_req(response, 200);

  int message_len = strlen(msg);
  send(client_fd, msg, message_len, 0);
}