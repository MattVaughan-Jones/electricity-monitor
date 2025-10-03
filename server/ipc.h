#ifndef IPC_H

#define IPC_H

#define MAX_RECORDING_NAME_LEN 64

struct ws_ipc_t {
    int ws_fd;
    char recording_name[MAX_RECORDING_NAME_LEN];
};

extern struct ws_ipc_t *shared_mem;

#endif