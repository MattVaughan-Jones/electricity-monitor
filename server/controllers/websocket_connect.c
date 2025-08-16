#include "../validators.h"
#include "http.h"
#include "sha1.h"
#include "websocket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int get_swk_from_header(char *swk_encoded, char *recv_buf) {
  char *req = malloc(strlen(recv_buf) + 1);
  strcpy(req, recv_buf);
  char *key_line = strstr(req, "Sec-WebSocket-Key:");
  char *key_start = key_line + strlen("Sec-WebSocket-Key:");
  while (*key_start == ' ')
    key_start++;

  char *key_end = strpbrk(key_line, "\r\n");
  if (key_end) {
    int swk_encoded_len = key_end - key_start;
    if (swk_encoded_len != SEC_WEBSOCKET_KEY_SIZE) {
      return 1;
    }
    swk_encoded = strncpy(swk_encoded, key_start, swk_encoded_len);
    swk_encoded[swk_encoded_len] = '\0';
    return 0;
  }
  return 1;
}

char *generate_swa(char *swk_encoded) {
  char *concatenated_key =
      malloc(strlen(swk_encoded) + strlen(MAGIC_WEBSOCKET_STRING) + 1);
  snprintf(concatenated_key,
           strlen(swk_encoded) + strlen(MAGIC_WEBSOCKET_STRING) + 1, "%s%s",
           swk_encoded, MAGIC_WEBSOCKET_STRING);

  SHA1_CTX sha;
  uint8_t results[20];

  int concatenated_key_len = strlen(concatenated_key);
  SHA1Init(&sha);
  SHA1Update(&sha, (uint8_t *)concatenated_key, concatenated_key_len);
  SHA1Final(results, &sha);

  /* Print the digest as one long hex value */
  // printf("hash: ");
  // for (concatenated_key_len = 0; concatenated_key_len < 20;
  //      concatenated_key_len++)

  // {
  //   printf("%02x", results[concatenated_key_len]);
  // }
  // printf("\n");

  char *swa_key = malloc(31);
  base64_encode(swa_key, results, strlen((char *)results));
  swa_key[31] = '\0';
  return swa_key;
}

void controller_websocket_connect(char *recv_buf, int client_fd) {
  int isValid = validate_sock_con_req(recv_buf);
  if (isValid != 0) {
    char *msg = build_req("Error: invalid socket connection request\n", 400);
    int message_len = strlen(msg);
    send(client_fd, msg, message_len, 0);
  } else {
    // TODO -
    // extract swk - done
    // append magic string - done
    // SHA-1 hash of the string -> 20 byte binary value
    // base 64 encode that hash === header

    char *swk_encoded = malloc(SEC_WEBSOCKET_KEY_SIZE);
    int get_swk_from_header_result;
    if ((get_swk_from_header_result =
             get_swk_from_header(swk_encoded, recv_buf)) != 0) {
      // TODO - probably need some better error handling for cases like this.
      // Would be nice to log an error which would somehow be caught and
      // return a 500 with that error string. Implement helper func for this.
      char *response =
          "Internal server error: unable to decode Sec-Websocket-Key\n";
      char *msg = build_req(response, 500);

      int message_len = strlen(msg);
      send(client_fd, msg, message_len, 0);
    }

    char *sec_websocket_accept_key = generate_swa(swk_encoded);

    char *res = ws_connection_upgrade_res(sec_websocket_accept_key);
    // char *res = ws_connection_upgrade_res();
    int res_len = strlen(res);
    send(client_fd, res, res_len, 0);
  }
}
