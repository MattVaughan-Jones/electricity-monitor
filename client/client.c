#include "client-validator.h"
#include "http-util.h"
#include "stream-data.h"
#include "ws-http-keys.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT "8080"
#define URL "localhost"
#define MAXDATASIZE 256 // max number of bytes we can get at once

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void) {
  struct addrinfo hints, *servinfo, *p;
  int getaddrinfo_status;
  int sock_fd;
  char s[INET6_ADDRSTRLEN];
  char buf[MAXDATASIZE];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((getaddrinfo_status = getaddrinfo(URL, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(getaddrinfo_status));
    return 1;
  }

  // loop through results and connect to the first
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
        -1) {
      perror("client: socket");
      continue;
    }
    if (inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s,
                  sizeof s) == NULL) {
      perror("inet_ntop");
      close(sock_fd);
    }
    printf("client: attempting to connect to %s\n", s);

    if (connect(sock_fd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("client: connect");
      close(sock_fd);
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    freeaddrinfo(servinfo);
    return 2;
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s,
            sizeof s);
  printf("client: connected to %s\n", s);

  freeaddrinfo(servinfo);

  char *sec_websocket_key = generate_swk();
  if (sec_websocket_key == NULL) {
    fprintf(stderr, "client: failed to generate sec_websocket_key\n");
    close(sock_fd);
    return 1;
  }

  char *msg = ws_connection_req("localhost", PORT, sec_websocket_key);
  if (msg == NULL) {
    fprintf(stderr, "client: failed to generate ws_connection_req\n");
    close(sock_fd);
    free(sec_websocket_key);
    return 1;
  }

  if (send(sock_fd, msg, strlen(msg), 0) == -1) {
    perror("send");
    free(msg);
    free(sec_websocket_key);
    close(sock_fd);
    return 1;
  }

  free(msg);

  int numbytes = recv(sock_fd, buf, MAXDATASIZE - 1, 0);
  if (numbytes == -1) {
    perror("recv");
    free(sec_websocket_key);
    close(sock_fd);
    return 1;
  }

  buf[numbytes] = '\0';

  printf("\nclient: received\n%s\n", buf);

  if (validate_wsa_res(buf, sec_websocket_key) != 0) {
    fprintf(stderr, "client: WebSocket validation failed\n");
    free(sec_websocket_key);
    close(sock_fd);
    return 1;
  }

  stream_data(sock_fd);

  free(sec_websocket_key);

  close(sock_fd);

  return 0;
}
