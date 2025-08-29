#include "base64.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sha1.h"
#include "ws-http-keys.h"

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