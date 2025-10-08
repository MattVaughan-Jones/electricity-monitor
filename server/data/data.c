#include "data.h"

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

int replace_character_occurrences(char *input, char unwanted, char replacement) {
  if (!input) {
    fprintf(stderr, "input to replace_character_occurrences is NULL\n");
    return -1;
  }

  if (unwanted == '\0') {
    fprintf(stderr, "Error: cannot replace null terminator\n");
    return -1;
  }

  int replacements_made = 0;
  for (int i = 0; input[i] != '\0'; i++) {
    if (input[i] == unwanted) {
      input[i] = replacement;
      replacements_made++;
    }
  }
  return replacements_made;
}

void replace_special_characters(char *input) {
  if (!input) {
    fprintf(stderr, "NULL recording_name passed to replace_special_characters\n");
    return;
  }

  char special_characters[] = {
    ' ', ':', '/', '\\', '*', '?', '<', '>', '|', '"', 
    '\'', '&', '%', '$', '#', '@', '!', '~', '`', '^',
    '\0'
  };

  for (int i = 0; special_characters[i] != '\0'; i++) {
    replace_character_occurrences(input, special_characters[i], '_');
  }
}

char *build_file_path(char *recording_name) {
  if (!recording_name) {
    fprintf(stderr, "NULL recording_name passed to build_file_path\n");
    return NULL;
  }

  if (strlen(recording_name) == 0 || strlen(recording_name) > MAX_RECORDING_NAME_LEN) {
    fprintf(stderr, "recording_name length of %zu is outside the bounds\n", strlen(recording_name));
    return NULL;
  }

  time_t rawtime;
  struct tm *timeinfo;
  char datetime[20];
  char *file_name_template = "%s/%s/%s_%s.json";
  char recording_name_copy[MAX_RECORDING_NAME_LEN + 1];

  const char *home_dir = getenv("HOME");
  if (!home_dir) {
    fprintf(stderr, "unable to find HOME dir\n");
    return NULL;
  }
  
  time (&rawtime);
  if (rawtime == (time_t)-1) {
    perror("time() failed");
    return NULL;
  }
  
  timeinfo = localtime (&rawtime);
  if (!timeinfo) {
    fprintf(stderr, "localtime() failed\n");
    return NULL;
  }
  
  if (strftime(datetime, sizeof(datetime), "%Y-%m-%dT%H.%M.%S", timeinfo) != 19) {
    fprintf(stderr, "strftime() failed to format datetime\n");
    return NULL;
  }

  strncpy(recording_name_copy, recording_name, strlen(recording_name) + 1);
  replace_special_characters(recording_name_copy);

  int file_name_len = snprintf(NULL, 0, file_name_template, home_dir, DATA_DIR, recording_name, datetime) + 1;
  if (file_name_len <= 0 || file_name_len > MAX_FILE_PATH_LEN) {
    fprintf(stderr, "file_name length of %d is outside the bounds\n", file_name_len);
    return NULL;
  }

  char *file_name = malloc(file_name_len);
  if (!file_name) {
    fprintf(stderr, "malloc failed for file_name\n");
    return NULL;
  }
  
  int result = snprintf(file_name, file_name_len, file_name_template, home_dir, DATA_DIR, recording_name_copy, datetime);
  if (result < 0 || result >= file_name_len) {
    fprintf(stderr, "snprintf failed to generate file name\n");
    free(file_name);
    return NULL;
  }

  return file_name;
}

FILE *make_file(char *file_name) {
  FILE *file = fopen(file_name, "w");
  if (!file) {
    fprintf(stderr, "failed to open file\n");
    return NULL;
  }

  return file;
}
