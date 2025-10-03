#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

void controller_start_recording(int client_fd, char *body);
void controller_stop_recording(int client_fd);

#endif