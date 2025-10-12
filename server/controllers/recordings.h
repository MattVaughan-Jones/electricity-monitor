#ifndef CONTROLLER_RECORDINGS_H
#define CONTROLLER_RECORDINGS_H

#define MAX_RECORDINGS_LIST_LEN 4096
#define MAX_FILE_SIZE 1 * 1024 *1024 // 1MB

void controller_get_all_recordings(int client_fd);
void controller_get_recording(int client_fd, char *recording_name);

#endif