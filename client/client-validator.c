#include "ws-http-keys.h"
#include <stdlib.h>
#include <string.h>

#include "http.h"

int validate_wsa_res(const char *res, const char *sec_websocket_key) {
  if (!res || !sec_websocket_key)
    return 1;

  char *sec_websocket_accept = generate_swa(sec_websocket_key);
  if (!sec_websocket_accept)
    return 1;

  char *expected_res = ws_connection_upgrade_res(sec_websocket_accept);
  if (!expected_res) {
    free(sec_websocket_accept);
    return 1;
  }

  int result = (strstr(res, expected_res) == NULL) ? 1 : 0;

  free(sec_websocket_accept);
  free(expected_res);

  return result;
}
