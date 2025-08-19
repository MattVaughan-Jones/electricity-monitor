#include "websocket.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sha1.h"

int base64_encode(char *output_buf, const unsigned char *input, int input_len) {

  if (!output_buf || !input) {
    fprintf(stderr, "passed NULL pointer to base64_encode\n");
    return -1;
  }

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

  return 0;
}

char *generate_swk() {
  static int seeded = 0;
  if (!seeded) {
    srand(time(NULL));
    seeded = 1;
  }

  char *buffer = malloc(SEC_WEBSOCKET_KEY_SIZE + 1);
  if (buffer == NULL) {
    fprintf(stderr, "failed to allocate sec_websocket_key\n");
    return NULL;
  }
  buffer[SEC_WEBSOCKET_KEY_SIZE] = '\0';

  unsigned char random_bytes[WEBSOCKET_KEY_INPUT_STR_LEN];
  for (int i = 0; i < WEBSOCKET_KEY_INPUT_STR_LEN; i++) {
    random_bytes[i] = rand() % 256;
  }

  if (base64_encode(buffer, random_bytes, WEBSOCKET_KEY_INPUT_STR_LEN) != 0) {
    fprintf(stderr, "failed to base64 encode\n");
    free(buffer);
    return NULL;
  }

  return buffer;
}

char *generate_swa(const char *swk_encoded) {
  char *concatenated_key =
      malloc(strlen(swk_encoded) + strlen(MAGIC_WEBSOCKET_STRING) + 1);
  if (!concatenated_key) {
    fprintf(stderr, "failed to allocate concatenated_key in generate_swa\n");
    return NULL;
  }

  snprintf(concatenated_key,
           strlen(swk_encoded) + strlen(MAGIC_WEBSOCKET_STRING) + 1, "%s%s",
           swk_encoded, MAGIC_WEBSOCKET_STRING);

  SHA1_CTX sha;
  uint8_t results[20];

  int concatenated_key_len = strlen(concatenated_key);
  SHA1Init(&sha);
  SHA1Update(&sha, (uint8_t *)concatenated_key, concatenated_key_len);
  SHA1Final(results, &sha);

  free(concatenated_key);

  /* Print the digest as one long hex value */
  // printf("hash: ");
  // for (int i = 0; i < 20;
  //      i++)

  // {
  //   printf("%02x", results[i]);
  // }
  // printf("\n");

  int base64_output_size = (20 + 2) / 3 * 4;

  char *swa = malloc(base64_output_size + 1);
  if (!swa) {
    fprintf(stderr, "failed to allocate swa in generate_swa\n");
    return NULL;
  }

  if (base64_encode(swa, results, 20) != 0) {
    fprintf(stderr, "failed to base64 encode\n");
    return NULL;
  }
  return swa;
}
