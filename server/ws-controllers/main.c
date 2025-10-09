#include "ws.h"
#include "../ipc.h"
#include "../data/data.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <time.h>
#include <string.h>

FILE *file = NULL;
char current_file_path[MAX_FILE_PATH_LEN] = {0};
int data_count = 0; // Track number of data entries written

void create_new_recording() {
  // Close old file if it exists
  if (file) {
    fclose(file);
    file = NULL;
  }
  
  // Open new file for the new recording
  file = make_file(shared_mem->file_path);
  if (file) {
    strncpy(current_file_path, shared_mem->file_path, MAX_FILE_PATH_LEN - 1);
    current_file_path[MAX_FILE_PATH_LEN - 1] = '\0';
    
    // Write complete JSON structure upfront
    fprintf(file, "{\n");
    fprintf(file, "  \"recording_name\": \"%s\",\n", shared_mem->recording_name);
    fprintf(file, "  \"data\": [\n");
    fprintf(file, "    "); // Start data array with proper indentation
    fflush(file);
    
    data_count = 0; // Reset counter for new recording
  }
}

void process_incoming_data(char *message) {
  if (!message) {
    fprintf(stderr, "message passed to process_incoming_data is NULL\n");
    return;
  }
  // use file_path from shared memory (set by controller at start)
  if (shared_mem->file_path[0] == '\0') {
    fprintf(stderr, "file_path not set in shared memory; cannot open file for writing\n");
    return;
  }

  // Check if we need to open a new file (new recording started)
  if (!file || strcmp(current_file_path, shared_mem->file_path) != 0) {
    create_new_recording();
  }
  // Write data to the file
  if (file) {
    if (data_count > 0) {
      fprintf(file, ",\n    "); // Add comma and newline for subsequent entries
    }
    fprintf(file, "%s", message);
    data_count++;
    fflush(file); // Ensure data is written immediately
  }
}

void close_json_structure(FILE *file) {
  if (file) {
    fprintf(file, "\n  ]\n");
    fprintf(file, "}\n");
    fclose(file);
    file = NULL;
    data_count = 0;
  }
}

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

    int result = poll(&pfd, 1, 50); // timeout in ms - shorter for faster disconnection detection

    time_t now = time(NULL);

    // If a recording is active (file open) but the controller has cleared the
    // file_path, finalize and close the JSON document immediately.
    if (shared_mem->file_path[0] == '\0') {
      close_json_structure(file);
    }

    if (result > 0 && (pfd.revents & POLLIN))
    {
      // Data available to read
      int bytes = recv(client_fd, buffer, sizeof(buffer), 0);

      if (bytes <= 0)
      {
        printf("ESP32 disconnected (connection closed)\n");
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
      else if (check_is_electricity_data(message))
      {
        process_incoming_data(message);
        printf("received message: %s\n", message);
      } else {
        fprintf(stderr, "WS controller received unrecognised message\n");
      }

      free(message);
    }

    // Send ping if it's time
    if (now - last_ping_time >= PING_INTERVAL)
    {
      if (send_ping(client_fd) < 0)
      {
        printf("Ping send from server failed. Disconnecting WS client\n");
        break;
      }
      last_ping_time = now;
    }

    // Check for pong timeout
    if (now - last_pong_time >= PONG_TIMEOUT)
    {
      printf("ESP32 disconnected - no pong response for %d seconds\n", PONG_TIMEOUT);
      break;
    }
  }

  // Close JSON structure if file is open
  close_json_structure(file);

  // Reset state
  shared_mem->ws_fd = -1; // reset websocket file descriptor
  shared_mem->ws_pid = -1; // reset websocket process ID
  memset(shared_mem->file_path, 0, MAX_FILE_PATH_LEN); // wipe file_path
  memset(shared_mem->recording_name, 0, MAX_RECORDING_NAME_LEN); // wipe recording_name
  memset(current_file_path, 0, MAX_FILE_PATH_LEN); // Reset current file path
  data_count = 0; // Reset data counter

  close(client_fd);
  exit(0); // Exit the forked process
}
