#include <stddef.h>
#include <stdint.h>

#ifndef WS_H
#define WS_H

#define WS_FRAME_HEADER_SIZE_2 2
#define WS_MASK_SIZE_4 4
#define WS_EXTENDED_LEN_2 2
#define WS_MAX_PAYLOAD_126 126
#define WS_MAX_PAYLOAD_65536 65536

typedef struct {
  unsigned char *frame_buf;
  size_t len;
} ws_frame_buf_t;

typedef struct {
  unsigned char first_byte;
  unsigned char second_byte;
  uint16_t extended_payload_len;
  uint32_t maybe_mask;
  unsigned char *payload;
  size_t payload_len;
  size_t frame_len;
} ws_frame_t;

ws_frame_buf_t *ws_encode(char *input_payload, const unsigned int should_mask);
void print_binary_bytes(const unsigned char *binary, int num_bytes);
char *ws_decode(const unsigned char *input, size_t input_len);
int mask_unmask_payload(ws_frame_t *frame);

#endif