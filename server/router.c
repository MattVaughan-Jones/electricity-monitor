#include "controllers/websocket_connect.h"
#include "http-util.h"
#include "http/http.h"
#include "controllers/device_control.h"
#include "controllers/recordings.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define MAX_RECEIVE_SIZE 1024
#define MAX_BODY_SIZE 1024

struct req_t
{
  char path[256];
  char method[10];
  char body[MAX_BODY_SIZE];
};

// Helper function to safely compare paths with bounds checking
static int path_matches(const char *path, const char *expected)
{
  if (!path || !expected)
  {
    return 0;
  }

  if (strlen(path) != strlen(expected))
  {
    return 0;
  }

  return strcmp(path, expected) == 0;
}

static int path_starts_with(const char *path, const char *prefix)
{
  if (!path || !prefix) {
    return 0;
  }

  size_t prefix_len = strlen(prefix);
  return strncmp(path, prefix, prefix_len) == 0;
}

static int parse_request(char *recv_buf, struct req_t *request)
{
  if (!recv_buf)
  {
    fprintf(stderr, "undefined argument passed to parse_request\n");
    return 1;
  }
  char version[20], first_line[256];

  char *newline = strchr(recv_buf, '\n');
  if (!newline)
    return 1;

  size_t first_line_len = newline - recv_buf;
  if (first_line_len > sizeof(first_line))
  {
    fprintf(stderr, "First line of HTTP request too long to parse. Max size is "
                    "255 characters\n");
    return 1;
  }

  strncpy(first_line, recv_buf, first_line_len);
  first_line[first_line_len] = '\0';

  if (sscanf(first_line, "%s %s %s", request->method, request->path, version) != 3)
  {
    fprintf(stderr, "Failed to parse first line of HTTP request\n");
    return 1;
  }

  char *start_of_body = strstr(recv_buf, "\r\n\r\n") + 4;
  size_t body_len = strlen(start_of_body);

  if (body_len > MAX_BODY_SIZE) {
    fprintf(stderr, "Body greater than MAX_BODY_SIZE. Cannot parse request\n");
    return -1;
  }

  strncpy(request->body, start_of_body, body_len);

  return 0;
}

void router(int client_fd)
{
  if (client_fd < 0)
  {
    fprintf(stderr, "Router error: Invalid client_fd\n");
    return;
  }

  struct req_t request = {0};

  char recv_buf[MAX_RECEIVE_SIZE];

  // receive the incoming message
  int num_bytes;
  if ((num_bytes = recv(client_fd, recv_buf, MAX_RECEIVE_SIZE, 0)) == -1)
  {
    perror("recv");
    send_response(client_fd, "Internal server error: failed to receive data", 500);
    return;
  }
  if (num_bytes >= MAX_RECEIVE_SIZE)
  {
    send_response(client_fd, "Request entity too large", 413);
    return;
  }
  if (num_bytes == 0)
  {
    // client disconnected
    return;
  }
  recv_buf[num_bytes] = '\0';

  if (parse_request(recv_buf, &request) != 0)
  {
    send_response(client_fd, "Internal server error: unable to parse path request", 400);
    return;
  }

  /*
   * ROUTER
  */
  // websocket-connect
  if (path_matches(request.path, "/websocket-connect"))
  {
    if (strncmp(request.method, "GET", strlen("GET")) == 0)
    {
      controller_websocket_connect(recv_buf, client_fd);
    }
    else
    {
      send_405_response(client_fd);
    }
  }
  // start-recording
  else if (path_matches(request.path, "/start-recording"))
  {
    if (strncmp(request.method, "POST", strlen("POST")) == 0)
    {
      controller_start_recording(client_fd, request.body);
    }
    else
    {
      send_405_response(client_fd);
    }
  }
  // stop-recording
  else if (path_matches(request.path, "/stop-recording"))
  {
    if (strncmp(request.method, "GET", strlen("GET")) == 0)
    {
      controller_stop_recording(client_fd);
    }
    else
    {
      send_405_response(client_fd);
    }
  }
  // recordings
  else if (path_matches(request.path, "/recordings"))
  {
    if (strncmp(request.method, "GET", strlen("GET")) == 0)
    {
      controller_get_all_recordings(client_fd);
    }
    else
    {
      send_405_response(client_fd);
    }
  }
  // recording/{file_name}
  else if (path_starts_with(request.path, "/recording/")) {
    if (strncmp(request.method, "GET", strlen("GET")) == 0) {
      char *recording_name = request.path + 11; // Skip "/recording/"
      controller_get_recording(client_fd, recording_name);
    } else {
      send_405_response(client_fd);
    }
  }
  else
  {
    send_404_response(client_fd);
  }
}
