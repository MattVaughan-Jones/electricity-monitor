#include "http.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "8080"
#define BACKLOG 1

int main(void) {
  int sock_fd, new_fd;
  struct addrinfo hints, *server_info;
  struct sockaddr_storage client_addr;
  socklen_t addr_size = sizeof client_addr;
  int getaddrinfo_status;

  memset(&hints, 0, sizeof hints);

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  hints.ai_flags = AI_PASSIVE;

  if ((getaddrinfo_status = getaddrinfo(NULL, PORT, &hints, &server_info)) !=
      0) {
    fprintf(stderr, "getaddrinfo error: %s\n",
            gai_strerror(getaddrinfo_status));
    exit(1);
  }

  if ((sock_fd = socket(server_info->ai_family, server_info->ai_socktype,
                        server_info->ai_protocol)) < 0) {
    perror("set sock_fd");
    exit(1);
  }

  if ((bind(sock_fd, server_info->ai_addr, server_info->ai_addrlen)) < 0) {
    perror("bind");
    exit(1);
  }

  freeaddrinfo(server_info);

  if ((listen(sock_fd, BACKLOG) != 0)) {
    perror("listen");
    exit(1);
  }

  printf("server: waiting for connections...\n");

  if ((new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &addr_size)) <
      0) {
    perror("accept");
    exit(1);
  }

  char *response = "Server says hello :)\n";
  int response_len = strlen(response);
  char *msg = build_headers(response, response_len);

  int message_len = strlen(msg);
  send(new_fd, msg, message_len, 0);

  close(new_fd);
  close(sock_fd);

  return 0;
}
