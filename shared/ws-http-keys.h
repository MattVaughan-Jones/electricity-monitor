#ifndef WS_HTTP_KEYS_H
#define WS_HTTP_KEYS_H

#define SEC_WEBSOCKET_KEY_SIZE 24
#define MAGIC_WEBSOCKET_STRING "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define WEBSOCKET_KEY_INPUT_STR_LEN 16

char *generate_swk();
char *generate_swa(const char *swk_encoded);

#endif