#include "../validators.h"
#include "http.h"

#include <string.h>
#include <sys/socket.h>

void controller_websocket_connect(char *recv_buf, int client_fd) {
  int isValid = validate_sock_con_req(recv_buf);
  if (isValid != 0) {
    char *msg = build_req("Error: invalid socket connection request\n", 400);
    int message_len = strlen(msg);
    send(client_fd, msg, message_len, 0);
  } else {
    // TODO - decode the Sec-Websocket-Key and calculate the
    // char *sec_websocket_accept = calculate_sec_websocket_accept(recv_buf);

    char *res = "temp websocket accept response \r\n";
    // char *res = ws_connection_upgrade_res();
    int res_len = strlen(res);
    send(client_fd, res, res_len, 0);
  }
}