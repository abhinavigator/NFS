#ifndef COMMON_H
#define COMMON_H

#include <netinet/in.h>

#define MAX_DATA_SIZE 1024
#define SERVER_PORT 12345

typedef struct {
    struct sockaddr_in address;
    int socket_fd;
    int conn_fd;
} connection_t;

#endif
