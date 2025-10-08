#include "../ipc.h"
#include "../data/data.h"
#include "ws.h"
#include "http-util.h"
#include "../http/http.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>

int set_recording_name(char *body)
{
  if (!body)
  {
    fprintf(stderr, "No body provided to set_recording_name");
    return -1;
  }

  if (!shared_mem)
  {
    fprintf(stderr, "shared_mem is NULL in set_recording_name");
    return -1;
  }

  // TODO - could do this more thoroughly to validate that this is actual JSON
  // eg: make sure there's a colon, make sure there are quote marks
  const char *end_of_recording_name_key = strstr(body, "recordingName") + strlen("recordingName");
  if (!end_of_recording_name_key)
    goto failed_to_allocate;
  const char *colon = strchr(end_of_recording_name_key, ':');
  if (!colon)
    goto failed_to_allocate;
  const char *start_of_recording_name = strchr(colon, '"') + 1;
  if (!start_of_recording_name)
    goto failed_to_allocate;
  const char *end_of_recording_name = strchr(start_of_recording_name, '"');
  if (!end_of_recording_name)
    goto failed_to_allocate;
  size_t recording_name_len = end_of_recording_name - start_of_recording_name;
  
  if (recording_name_len > MAX_RECORDING_NAME_LEN) {
    fprintf(stderr, "unable to set recording name. Likely error with JSON body format");
    return -1;
  }

  if (recording_name_len == 0)
  {
    // TODO - extract all file ops to a separate module
    char *file_path = build_file_path("unnamed");
    if (!file_path) return -1;
    strncpy(shared_mem->file_path, file_path, MAX_FILE_PATH_LEN - 1);
    free(file_path);
  }

  char name_buf[MAX_RECORDING_NAME_LEN + 1];
  strncpy(name_buf, start_of_recording_name, recording_name_len);
  // create name_buf so that we gaurantee passing a null terminated string to build_file_path
  name_buf[recording_name_len] = '\0';
  char *file_path = build_file_path(name_buf);
  if (!file_path) return -1;
  strncpy(shared_mem->file_path, file_path, MAX_FILE_PATH_LEN - 1);
  shared_mem->file_path[MAX_FILE_PATH_LEN - 1] = '\0';
  free(file_path);

  return 0;
failed_to_allocate:
{
  fprintf(stderr, "failed to allocate a char * when setting recording name");
  return -1;
}
}

void controller_start_recording(int client_fd, char *body)
{
  char *error_msg;
  int status = 200;

  if (shared_mem->ws_fd == -1)
  {
    printf("Cannot start device because it is not connected\n");
    error_msg = "Cannot start device because it is not connected";
    status = 409;
    goto failed_to_start;
  }

  char *start_recording = "{\"action\":\"start_recording\"}";
  ws_frame_buf_t *ws_frame_buf = ws_encode(start_recording, 1);
  if (!ws_frame_buf || !ws_frame_buf->frame_buf || !ws_frame_buf->len)
  {
    fprintf(stderr, "failed to allocate len\n");
    if (ws_frame_buf->frame_buf)
    {
      free(ws_frame_buf->frame_buf);
    }
    if (ws_frame_buf)
    {
      free(ws_frame_buf);
    }
    error_msg = "Internal server error";
    status = 500;
    goto failed_to_start;
  }

  int bytes_sent = send(shared_mem->ws_fd, ws_frame_buf->frame_buf, ws_frame_buf->len, 0);
  if (bytes_sent < 0)
  {
    perror("send failed");
    free(ws_frame_buf->frame_buf);
    free(ws_frame_buf);
    error_msg = "Internal server error";
    status = 500;
    goto failed_to_start;
  }

  // Set recording name
  if (set_recording_name(body) != 0) {
    error_msg = "Request body malformed";
    status = 400;
    goto failed_to_start;
  }

  free(ws_frame_buf->frame_buf);
  free(ws_frame_buf);

  send_response(client_fd, "recording started", 200);
  return;

failed_to_start:
{
  send_response(client_fd, error_msg, status);
  return;
}
}

void controller_stop_recording(int client_fd)
{
  char *error_msg;
  int status = 200;
  if (shared_mem->ws_fd == -1)
  {
    printf("Cannot stop device because it is not connected\n");
    error_msg = "Cannot stop device because it is not connected";
    status = 404;
    goto failed_to_stop;
  }

  char *stop_recording = "{\"action\":\"stop_recording\"}";
  ws_frame_buf_t *ws_frame_buf = ws_encode(stop_recording, 1);
  if (!ws_frame_buf || !ws_frame_buf->frame_buf || !ws_frame_buf->len)
  {
    fprintf(stderr, "failed to allocate len\n");
    if (ws_frame_buf->frame_buf)
    {
      free(ws_frame_buf->frame_buf);
    }
    if (ws_frame_buf)
    {
      free(ws_frame_buf);
    }
    error_msg = "Internal server error";
    status = 500;
    goto failed_to_stop;
  }

  printf("sending stop_recording signal\n");

  int bytes_sent = send(shared_mem->ws_fd, ws_frame_buf->frame_buf, ws_frame_buf->len, 0);
  if (bytes_sent < 0)
  {
    perror("send failed");
    free(ws_frame_buf->frame_buf);
    free(ws_frame_buf);
    error_msg = "Internal server error";
    status = 500;
    goto failed_to_stop;
  }

  free(ws_frame_buf->frame_buf);
  free(ws_frame_buf);

  memset(shared_mem->file_path, 0, MAX_FILE_PATH_LEN); // wipe file_path

  send_response(client_fd, "recording stopped", 200);
  return;

failed_to_stop:
{
  send_response(client_fd, error_msg, status);
  return;
}
}
