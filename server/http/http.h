#ifndef HTTP_H
#define HTTP_H

void send_response(const int client_fd, const char *message, const int status);
void send_404_response(int client_fd);
void send_405_response(int client_fd);

#endif