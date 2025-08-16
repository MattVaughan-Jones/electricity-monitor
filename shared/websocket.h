#ifndef WEBSOCKET_H
#define WEBSOCKET_H
#define SEC_WEBSOCKET_KEY_SIZE 24

int generate_swk(char *buffer);
char *calculate_sec_websocket_accept(const char *recv_buf);

#endif