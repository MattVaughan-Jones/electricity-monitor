#include "websocket.h"
#include <stdio.h>
#include <string.h>

#include "http.h"

int validate_wsa_res(const char *res, const char *sec_websocket_key) {
  char *sec_websocket_accept = generate_swa(sec_websocket_key);
  printf("client-generated swa: %s", sec_websocket_accept);

  char *expected_res = ws_connection_upgrade_res(sec_websocket_accept);

  if (strstr(res, expected_res) != 0) {
    return 1;
  }
  return 0;
}