#ifndef HEADERS_H
#define HEADERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> 
#include <sys/stat.h> // For stat
#include <pthread.h>
#include <dirent.h>
#include <stdbool.h>
#include <semaphore.h>



#define MAX_DATA_SIZE 1024
#define SERVER_PORT 12345
#define NM_PORT 10000
#define BUFFER_SIZE 1024
#define PATH_LENGTH 1024

typedef struct {
    struct sockaddr_in address;
    int socket_fd;
    int conn_fd;
} connection_t;

#include "commstruct.h"
#include "hashtable.h"
#include "storage_server.h"
#include "operations.h"
#include "naming_server.h"


StorageServerNode* add_storage_server(char* ip, int nm_port, int client_port, char* path) ;

#endif