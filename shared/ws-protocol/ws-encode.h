#ifndef WS_ENCODE_H
#define WS_ENCODE_H

#include <stddef.h>
typedef struct {
  unsigned char *frame_buf;
  size_t len;
} ws_frame_buf_t;

ws_frame_buf_t *ws_encode(char *input_payload, const unsigned int should_mask);
void print_binary_bytes(const unsigned char *binary, int num_bytes);
#endif