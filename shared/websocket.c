#include "websocket.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
