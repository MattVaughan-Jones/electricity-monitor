#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

void stream_data(int sock_fd) {
  int number_of_sends = 0;
  while (number_of_sends < 2) {
    sleep(2);
    char *template = "Hi %d";
    int message_len = snprintf(NULL, 0, template, number_of_sends) + 1;
    char *message = malloc(message_len);
    snprintf(message, message_len, template, number_of_sends);

    send(sock_fd, message, 5, 0);
    number_of_sends++;
  }
}