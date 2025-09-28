#include "ws.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>

// Send a ping message to the client
int send_ping(int client_fd) {
    // Input validation
    if (client_fd < 0) {
        fprintf(stderr, "send_ping: Invalid client_fd (%d)\n", client_fd);
        return -1;
    }
    
    const char *ping = "{\"type\":\"ping\"}";
    ws_frame_buf_t *ping_frame = ws_encode(ping, 1);
    
    if (!ping_frame || !ping_frame->frame_buf || ping_frame->len == 0) {
        fprintf(stderr, "Failed to encode ping frame\n");
        if (ping_frame) {
            free(ping_frame->frame_buf);
            free(ping_frame);
        }
        return -1;
    }
    
    int bytes_sent = send(client_fd, ping_frame->frame_buf, ping_frame->len, 0);
    
    free(ping_frame->frame_buf);
    free(ping_frame);
    
    if (bytes_sent < 0) {
        fprintf(stderr, "send_ping: send failed for fd %d: %s\n", client_fd, strerror(errno));
        return -1;
    }
    return 0;
}

// Check if a message is a pong response
int check_is_pong(const char *message) {
    if (!message) {
        return 0;
    }
    
    return (strstr(message, "{\"type\":\"pong\"}") != NULL);
}

// Check if a message is a ping request
int check_is_ping(const char *message) {
    if (!message) {
        return 0;
    }
    
    return (strstr(message, "{\"type\":\"ping\"}") != NULL);
}

// Send a pong response to the client
int send_pong(int client_fd) {
    // Input validation
    if (client_fd < 0) {
        fprintf(stderr, "send_pong: Invalid client_fd (%d)\n", client_fd);
        return -1;
    }
    
    const char *pong = "{\"type\":\"pong\"}";
    ws_frame_buf_t *pong_frame = ws_encode(pong, 1);
    
    if (!pong_frame || !pong_frame->frame_buf || pong_frame->len == 0) {
        fprintf(stderr, "Failed to encode pong frame\n");
        if (pong_frame) {
            free(pong_frame->frame_buf);
            free(pong_frame);
        }
        return -1;
    }
    
    int bytes_sent = send(client_fd, pong_frame->frame_buf, pong_frame->len, 0);
    
    free(pong_frame->frame_buf);
    free(pong_frame);
    
    if (bytes_sent < 0) {
        perror("send pong failed");
        return -1;
    }
    return 0;
}
