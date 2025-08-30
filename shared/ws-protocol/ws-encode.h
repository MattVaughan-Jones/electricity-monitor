#ifndef WS_ENCODE_H
#define WS_ENCODE_H

typedef struct {
  unsigned char *frame_buf;
  int len;
} ws_frame_buf_t;

ws_frame_buf_t *ws_encode(const char *input_payload,
                          const unsigned int should_mask);
void print_binary_bytes(const unsigned char *binary, int num_bytes);
#endif