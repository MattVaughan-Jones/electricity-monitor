#include "ws.h"

#include <stdio.h>

void print_binary_byte(const unsigned char binary) {
  for (int i = 7; i >= 0; i--) {
    printf("%d", 1 & (binary >> i));
  }
  printf("\n");
}

void print_binary_bytes(const unsigned char *binary, int num_bytes) {
  if (!binary || num_bytes <= 0) {
    return;
  }
  for (int byte_index = 0; byte_index < num_bytes; byte_index++) {
    print_binary_byte(binary[byte_index]);
  }
  printf("\n");
}

int mask_unmask_payload(ws_frame_t *frame) {
  if (!frame->payload || !frame->maybe_mask) {
    return -1;
  }

  size_t actual_payload_len = frame->extended_payload_len
                                  ? frame->extended_payload_len
                                  : frame->payload_len;

  for (size_t i = 0; i < actual_payload_len; i++) {
    unsigned char mask_byte =
        (frame->maybe_mask >> (8 * (3 - i % WS_MASK_SIZE_4))) & 0xFF;
    frame->payload[i] ^= mask_byte;
  }
  return 0;
}
