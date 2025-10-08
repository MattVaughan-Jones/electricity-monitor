#include "../ipc.h"

#include <string.h>
#include <stdio.h>

#ifndef DATA_H
#define DATA_H

#define DATA_DIR "Projects/electricity-monitor/server/storage"
#define DATETIME_STRING_LEN 20 // YYYY-mm-ddThh-mm-ss\0

char *build_file_path(char *recording_name);
int replace_character_occurrences(char *input, char unwanted, char replacement);
void replace_special_characters(char *input);
FILE *make_file(char *recording_name);
int check_is_electricity_data(char *message);

#endif