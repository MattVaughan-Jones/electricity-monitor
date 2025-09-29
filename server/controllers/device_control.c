#include "../ipc.h"
#include "ws.h"
#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>

void controller_start_recording(int client_fd)
{
  char *error_msg;
  int status = 200;
  if (*ws_fd == -1)
  {
    printf("Cannot start device because it is not connected\n");
    error_msg = "Cannot start device because it is not connected";
    status = 404;
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

  int bytes_sent = send(*ws_fd, ws_frame_buf->frame_buf, ws_frame_buf->len, 0);
  if (bytes_sent < 0)
  {
    perror("send failed");
    free(ws_frame_buf->frame_buf);
    free(ws_frame_buf);
    error_msg = "Internal server error";
    status = 500;
    goto failed_to_start;
  }

  free(ws_frame_buf->frame_buf);
  free(ws_frame_buf);

  char *success_res = build_req("recording started", 200);
  if (success_res == NULL)
  {
    fprintf(stderr, "unable to construct 'recording started' response\n");
    return;
  }
  if (send(client_fd, success_res, strlen(success_res), 0) == -1)
  {
    perror("send");
    free(success_res);
    return;
  }
  return;

failed_to_start:
{
  char *msg = build_req(error_msg, status);
  if (msg == NULL)
  {
    fprintf(stderr, "unable to build failed_to_start response message\n");
    return;
  }
  if (send(client_fd, msg, strlen(msg), 0) == -1)
  {
    perror("send");
    free(msg);
    return;
  }
  free(msg);
  return;
}
}

void controller_stop_recording(int client_fd)
{
  char *error_msg;
  int status = 200;
  if (*ws_fd == -1)
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

  int bytes_sent = send(*ws_fd, ws_frame_buf->frame_buf, ws_frame_buf->len, 0);
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

  char *success_res = build_req("recording stopped", 200);
  if (success_res == NULL)
  {
    fprintf(stderr, "unable to construct 'recording stoped' response\n");
    return;
  }
  if (send(client_fd, success_res, strlen(success_res), 0) == -1)
  {
    perror("send");
    free(success_res);
    return;
  }
  free(success_res);
  return;

failed_to_stop:
{
  char *msg = build_req(error_msg, status);
  if (msg == NULL)
  {
    fprintf(stderr, "unable to build failed_to_stop response message\n");
    return;
  }
  if (send(client_fd, msg, strlen(msg), 0) == -1)
  {
    perror("send");
    free(msg);
    return;
  }
  free(msg);
  return;
}
}
