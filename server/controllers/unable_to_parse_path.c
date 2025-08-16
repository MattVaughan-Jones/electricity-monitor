#include "http.h"
#include <string.h>
#include <sys/socket.h>

void controller_unable_to_parse_path(int client_fd) {
  char *response = "Internal server error: unable to parse path\n";
  char *msg = build_req(response, 500);

  int message_len = strlen(msg);
  send(client_fd, msg, message_len, 0);
}