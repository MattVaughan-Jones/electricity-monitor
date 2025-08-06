#include "http.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

#define PORT "8080"
#define URL "localhost"
#define MAXDATASIZE 120 // max number of bytes we can get at once

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
    struct addrinfo hints, *servinfo, *p;
    int getaddrinfo_status;
    int sock_fd;
    char s[INET6_ADDRSTRLEN];
    char buf[MAXDATASIZE];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((getaddrinfo_status = getaddrinfo(URL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(getaddrinfo_status));
        return 1;
    }

    // loop through results and connect to the first
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }
        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
        printf("client: attempting to connect to %s\n", s);

        if (connect(sock_fd, p->ai_addr, p->ai_addrlen) == -1)
        {
            perror("client: connect\n");
            close(sock_fd);
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("client: connected to %s\n", s);

    freeaddrinfo(servinfo);

    char *response = "";
    int response_len = strlen(response);
    char *msg = build_headers(response, response_len);

    send(sock_fd, msg, strlen(msg), 0);

    int numbytes;
    if ((numbytes = recv(sock_fd, buf, MAXDATASIZE - 1, 0)) == -1)
    {
        perror("recv\n");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("client: received '%s'\n", buf);

    close(sock_fd);

    return 0;
}
