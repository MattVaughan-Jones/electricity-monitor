#include "ws.h"
#include "../ipc.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <time.h>
#include <string.h>

void handle_websocket_communication(int client_fd)
{
  if (client_fd <= 0)
  {
    fprintf(stderr, "Websocket controller error: Invalid client_fd");
    return;
  }

  unsigned char buffer[1024];
  time_t last_ping_time = time(NULL);
  time_t last_pong_time = time(NULL);
  const int PING_INTERVAL = 5;   // Send ping every 5 seconds
  const int PONG_TIMEOUT = 15;   // Timeout after 15 seconds

  while (1)
  {
    struct pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN; // Monitor for incoming data

    int result = poll(&pfd, 1, 200); // timeout in ms

    time_t now = time(NULL);

    if (result > 0 && (pfd.revents & POLLIN))
    {
      // Data available to read
      int bytes = recv(client_fd, buffer, sizeof(buffer), 0);

      if (bytes <= 0)
      {
        printf("ESP32 disconnected (connection closed)\n");
        shared_mem->ws_fd = -1;
        memset(shared_mem->recording_name, 0, MAX_RECORDING_NAME_LEN);
        printf("recording_name after disconnect: %s\n", shared_mem->recording_name);
        break;
      }

      char *message = ws_decode(buffer, bytes);
      if (!message)
      {
        fprintf(stderr, "Websocket controller error: Unable to decode buffer\n");
        continue;
      }

      // Check if it's a pong response
      if (check_is_pong(message))
      {
        last_pong_time = now;
      }
      else
      {
        printf("received message: %s\n", message);
      }

      free(message);
    }

    // Send ping if it's time
    if (now - last_ping_time >= PING_INTERVAL)
    {
      if (send_ping(client_fd) < 0)
      {
        printf("Ping send from server failed. Disconnecting WS client\n");
        shared_mem->ws_fd = -1;
        memset(shared_mem->recording_name, 0, MAX_RECORDING_NAME_LEN);
        break;
      }
      last_ping_time = now;
    }

    // Check for pong timeout
    if (now - last_pong_time >= PONG_TIMEOUT)
    {
      printf("ESP32 disconnected - no pong response for %d seconds\n", PONG_TIMEOUT);
      shared_mem->ws_fd = -1;
      memset(shared_mem->recording_name, 0, MAX_RECORDING_NAME_LEN);
      break;
    }
  }

  close(client_fd);
  exit(0); // Exit the forked process
}
