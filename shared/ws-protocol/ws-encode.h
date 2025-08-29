#ifndef WS_ENCODE_H
#define WS_ENCODE_H

unsigned char *ws_encode(const char *input_payload,
                         const unsigned int should_mask);
void print_binary_bytes(const unsigned char *binary, int num_bytes);
#endif