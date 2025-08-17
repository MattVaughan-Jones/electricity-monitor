#include "websocket.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sha1.h"

void base64_encode(char *output_buf, const unsigned char *input,
                   int input_len) {
  // Base64 encoding table
  const char base64_chars[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  int j = 0;

  // Process input in groups of 3 bytes
  for (int i = 0; i < input_len; i += 3) {
    uint32_t octet_1 = input[i];
    uint32_t octet_2 = (i + 1 < input_len) ? input[i + 1] : 0;
    uint32_t octet_3 = (i + 2 < input_len) ? input[i + 2] : 0;

    uint32_t triple = (octet_1 << 16) + (octet_2 << 8) + octet_3;

    output_buf[j++] = base64_chars[(triple >> 18) & 63];
    output_buf[j++] = base64_chars[(triple >> 12) & 63];
    output_buf[j++] = base64_chars[(triple >> 6) & 63];
    output_buf[j++] = base64_chars[triple & 63];
  }

  // Add padding for incomplete triplets
  int padding = (3 - (input_len % 3)) % 3;
  for (int i = 0; i < padding; i++) {
    output_buf[j - 1 - i] = '=';
  }

  output_buf[j] = '\0';
}

int generate_swk(char *buffer) {
  static int seeded = 0;
  if (!seeded) {
    srand(time(NULL));
    seeded = 1;
  }

  unsigned char random_bytes[WEBSOCKET_KEY_INPUT_STR_LEN];
  for (int i = 0; i < WEBSOCKET_KEY_INPUT_STR_LEN; i++) {
    random_bytes[i] = rand() % 256;
  }

  base64_encode(buffer, random_bytes, WEBSOCKET_KEY_INPUT_STR_LEN);

  return 0;
}

char *generate_swa(const char *swk_encoded) {
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

  char *swa = malloc(31);
  base64_encode(swa, results, strlen((char *)results));
  swa[31] = '\0';
  return swa;
}
