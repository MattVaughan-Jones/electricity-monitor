#include <stdint.h>
#include <stdio.h>

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
