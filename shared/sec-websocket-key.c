#include "sec-websocket-key.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int generate_swk(char *buffer) {
  // Base64 encoding table
  const char base64_chars[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  static int seeded = 0;
  if (!seeded) {
    srand(time(NULL));
    seeded = 1;
  }

  unsigned char random_bytes[16];
  for (int i = 0; i < 16; i++) {
    random_bytes[i] = rand() % 256;
  }

  for (int i = 0, j = 0; i < 16;) {
    uint32_t octet_1 = i < 16 ? random_bytes[i++] : 0;
    uint32_t octet_2 = i < 16 ? random_bytes[i++] : 0;
    uint32_t octet_3 = i < 16 ? random_bytes[i++] : 0;

    uint32_t triple = (octet_1 << 16) + (octet_2 << 8) + octet_3;

    buffer[j++] = base64_chars[(triple >> 18) & 63];
    buffer[j++] = base64_chars[(triple >> 12) & 63];
    buffer[j++] = base64_chars[(triple >> 6) & 63];
    buffer[j++] = base64_chars[(triple) & 63];
  }
  buffer[SEC_WEBSOCKET_KEY_SIZE] = '\0';

  return 0;
}