#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "8080"
#define BACKLOG 1

int main(void) {
  int sock_fd, new_fd;
  struct addrinfo hints, *res;
  char ipstr[INET6_ADDRSTRLEN];
  struct sockaddr_storage client_addr;
  socklen_t addr_size = sizeof client_addr;

  memset(&hints, 0, sizeof hints);

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  hints.ai_flags = AI_PASSIVE;

  int getaddrinfo_status;
  if ((getaddrinfo_status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n",
            gai_strerror(getaddrinfo_status));
    exit(1);
  }

  if ((sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) <
      0) {
    perror("set sock_fd");
    exit(1);
  }

  struct sockaddr_storage *addr;

  if ((bind(sock_fd, res->ai_addr, res->ai_addrlen)) < 0) {
    perror("bind");
    exit(1);
  }

  // struct sockaddr_in *addr_in = (struct sockaddr_in *)res->ai_addr;
  // inet_ntop(res->ai_family, &(addr_in->sin_addr), ipstr, sizeof ipstr);
  // printf("IP address: %s: \n", ipstr);

  if ((listen(sock_fd, BACKLOG) != 0)) {
    perror("listen");
    exit(1);
  }

  if ((new_fd =
           accept(sock_fd, (struct sockaddr *)&client_addr, &addr_size) < 0)) {
    perror("accept");
    exit(1);
  }

  char *msg = "Server says hello :)";
  int len, bytes_sent;
  len = strlen(msg);
  bytes_sent = send(sock_fd, msg, len, 0);

  close(sock_fd);

  freeaddrinfo(res); // Should I do this?
  return 0;
}
