#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#define SEC_WEBSOCKET_KEY_SIZE 24
#define MAGIC_WEBSOCKET_STRING "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define WEBSOCKET_KEY_INPUT_STR_LEN 16

int generate_swk(char *buffer);
char *generate_swa(const char *swk_encoded);
void base64_encode(char *output_buf, const unsigned char *input, int input_len);

#endif