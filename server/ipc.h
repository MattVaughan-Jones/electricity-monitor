#ifndef IPC_H

#define IPC_H

#define MAX_RECORDING_NAME_LEN 64
#define MAX_FILE_PATH_LEN 512

struct ws_ipc_t {
    int ws_fd;
    char file_path[MAX_FILE_PATH_LEN];
};

extern struct ws_ipc_t *shared_mem;

#endif