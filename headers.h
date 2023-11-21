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
#include <sys/wait.h>
// #include <math.h>



#define MAX_DATA_SIZE 1024
#define SERVER_PORT 12345
#define NM_PORT 10000
#define BUFFER_SIZE 1024
#define PATH_LENGTH 1024
#define IP "127.0.0.1"

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


StorageServerNode* add_storage_server(char* ip, int nm_port, int client_port, char* path, int num) ;

#define MAX_SIZE 20
#define PATH_LENGTH 1024

typedef struct details_node details_node;

struct details_node {
    StorageServerNode* SSnode;
    // int SS_port;
    // int client_port;
    char path[PATH_LENGTH];
};

typedef struct list {
    details_node Details[MAX_SIZE];
    int latest_index;
    int oldest_index;
    int itemCount;
}list;

// Function to initialize the list
void initializeList(struct list *lst) {
    lst->latest_index = -1;
    lst->oldest_index = 0;
    lst->itemCount = 0;
}

// Function to check if a path already exists in the list
int pathExists(struct list *lst, char *path) {
    int start_index = lst->latest_index;
    int end_index = (lst->oldest_index - 1 + MAX_SIZE) % MAX_SIZE; // Calculate the end index

    for (int i = start_index; i != end_index; i = (i - 1 + MAX_SIZE) % MAX_SIZE) {
        if (strcmp(lst->Details[i].path, path) == 0) {
            return i; // Return index if path exists
        }
    }

    if (strcmp(lst->Details[end_index].path, path) == 0) {
        return end_index; // Check the last node for a match
    }

    return -1; // Return -1 if path doesn't exist
}



// Function to remove node from a specific index in the list and eliminate the gap
void removeNode(struct list *lst, int index) {
    int i = index;
    while (i != lst->latest_index) {
        lst->Details[i] = lst->Details[(i + 1) % MAX_SIZE];
        i = (i + 1) % MAX_SIZE;
    }
    lst->latest_index = (lst->latest_index - 1 + MAX_SIZE) % MAX_SIZE;
    lst->itemCount--;
}

// Function to add a new node to the list with circular handling
void addNode(struct list *lst, details_node newNode) {
    int index = pathExists(lst, newNode.path);
    
    if (index != -1) {
        removeNode(lst, index);
    }
    
    if (lst->itemCount == MAX_SIZE) {
        lst->oldest_index = (lst->oldest_index + 1) % MAX_SIZE; // Remove the oldest node
        lst->itemCount--;
    }

    lst->latest_index = (lst->latest_index + 1) % MAX_SIZE;
    lst->Details[lst->latest_index] = newNode; // Add the new node at the latest index
    lst->itemCount++;
}
#endif