#include "router.h"
#include "ipc.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/mman.h>

#define PORT "8080"
#define BACKLOG 1

struct ws_ipc_t *shared_mem = NULL;

void sigchld_handler(int s)
{
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
  int sock_fd, new_fd;
  struct addrinfo hints, *server_info;
  struct sockaddr_storage client_addr;
  socklen_t addr_size = sizeof client_addr;
  int getaddrinfo_status;
  char s[INET6_ADDRSTRLEN];
  struct sigaction sa;

  memset(&hints, 0, sizeof hints);

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  hints.ai_flags = AI_PASSIVE;

  shared_mem = mmap(NULL, sizeof(struct ws_ipc_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (shared_mem == MAP_FAILED) {
    perror("mmap failed");
    exit(1);
  }
        shared_mem->ws_fd = -1; // Initialise to -1 to indicate no fd assigned
        shared_mem->ws_pid = -1; // Initialise to -1 to indicate no process assigned
        memset(shared_mem->file_path, 0, MAX_FILE_PATH_LEN); // Initialise to empty string
        memset(shared_mem->recording_name, 0, MAX_RECORDING_NAME_LEN); // Initialise to empty string

  if ((getaddrinfo_status = getaddrinfo(NULL, PORT, &hints, &server_info)) !=
      0)
  {
    fprintf(stderr, "getaddrinfo error: %s\n",
            gai_strerror(getaddrinfo_status));
    exit(1);
  }

  if ((sock_fd = socket(server_info->ai_family, server_info->ai_socktype,
                        server_info->ai_protocol)) < 0)
  {
    perror("socket");
    exit(1);
  }

  int yes = 1;
  if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
  {
    perror("setsockopt");
    exit(1);
  }

  if (bind(sock_fd, server_info->ai_addr, server_info->ai_addrlen) < 0)
  {
    perror("bind");
    exit(1);
  }

  freeaddrinfo(server_info);

  if (listen(sock_fd, BACKLOG != 0))
  {
    perror("listen");
    exit(1);
  }

  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
      perror("sigaction");
      exit(1);
  }

  printf("server: waiting for connections...\n\n");

  while (1)
  {
    if ((new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &addr_size)) <
        0)
    {
      perror("accept");
      exit(1);
    }
  
    inet_ntop(client_addr.ss_family,
              get_in_addr((struct sockaddr *)&client_addr),
              s, sizeof s);
    printf("server: got connection from %s\n", s);
  
    if (!fork()) { // this is the child process
      close(sock_fd); // child doesn't need the listener
      (void)router(new_fd);
      exit(0);
    }
  }
  close(sock_fd);

  return 0;
}
