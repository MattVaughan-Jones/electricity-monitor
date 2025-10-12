#include "recordings.h"
#include "../http/http.h"
#include "../ipc.h"

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int is_ignored_file(char *d_name) {
  return strcmp(d_name, ".") == 0 || strcmp(d_name, "..") == 0;
}

int is_allowed_file_name(char *file_name) {
  for (char *p = file_name; *p; p++) {
    if (!isalnum(*p) && *p != '_' && *p != '-' && *p != '.') {
      return 0;
    }
  }
  return 1;
}

void controller_get_all_recordings(int client_fd) {
  char recordings_list[MAX_RECORDINGS_LIST_LEN-1] = {0};
  DIR *recording_storage_dir;
  struct dirent *entry;

  recording_storage_dir = opendir("./storage");
  if (recording_storage_dir == NULL) {
    send_response(client_fd, "Internal server error: failed to locate recording data", 500);
    return;
  }

  recordings_list[0] = '[';
  int first_entry_made = 0;
  while (first_entry_made == 0 && (entry = readdir(recording_storage_dir)) != NULL) {
    if (!is_ignored_file(entry->d_name)) {
      strcat(recordings_list, "\"");
      strcat(recordings_list, entry->d_name);
      strcat(recordings_list, "\"");
      first_entry_made++;
    }
  }

  while ((entry = readdir(recording_storage_dir)) != NULL) {
    // Exclude "." and ".." which represent the current and parent directories
    if (!is_ignored_file(entry->d_name)) {
      if (strlen(recordings_list) + strlen(entry->d_name) + 5 > MAX_RECORDINGS_LIST_LEN) { // 5 for `"", ` and a `]`
        printf("recordings_list buffer would overflow, stopping\n");
        send_response(client_fd, "Internal server error: failed to locate recording data", 500);
        break;
      }
      strcat(recordings_list, ", ");
      strcat(recordings_list, "\"");
      strcat(recordings_list, entry->d_name);
      strcat(recordings_list, "\"");
    }
  }

  strcat(recordings_list, "]");

  send_response(client_fd, recordings_list, 200);
  closedir(recording_storage_dir);
}

void controller_get_recording(int client_fd, char *file_name) {
  char file_path[10 + MAX_RECORDING_NAME_LEN + 25] = {0};

  if (!strstr(file_name, ".json")) {
    send_response(client_fd, "Invalid file type", 400);
    return;
  }

  if (!is_allowed_file_name(file_name)) {
    send_response(client_fd, "Invalid recording name", 400);
    return;
  }

  strcat(file_path, "./storage/");
  strcat(file_path, file_name);

  FILE* file = fopen(file_path, "rb");
  if (file == NULL) {
    perror("Error opening file");
    send_response(client_fd, "File not found", 404);
    return;
  }

  // Get file size
  fseek(file, 0, SEEK_END);
  unsigned long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  if (file_size > MAX_FILE_SIZE) {
    send_response(client_fd, "Internal server error: file too large to send", 500);
    fclose(file);
    return;
  }
  
  char *buffer = malloc(file_size + 1);
  if (!buffer) {
    send_response(client_fd, "Internal server error: memory allocation failed", 500);
    fclose(file);
    return;
  }
  
  // Read file into buffer
  size_t bytes_read = fread(buffer, 1, file_size, file);
  if (bytes_read != file_size) {
    perror("Error reading file");
    send_response(client_fd, "Internal server error: error reading file", 500);
    free(buffer);
    fclose(file);
    return;
  }
  
  buffer[bytes_read] = '\0';

  send_response(client_fd, buffer, 200);

  free(buffer);
  fclose(file);
}