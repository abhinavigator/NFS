#include "headers.h"



// Linked list node to store Storage Server information
typedef struct StorageServerNode {
    char ip_address[INET_ADDRSTRLEN];
    int nm_port;
    int client_port;
    int ss_port;
    char path[BUFFER_SIZE][PATH_LENGTH];
    int num_paths;
    struct StorageServerNode *next;
    pthread_t SSthread;
    sem_t SSsem;
} StorageServerNode;

extern StorageServerNode *head ;


typedef struct SSthread_arg {
    int port;                      //  TO BE COMPLETED
}SSthread_arg;

typedef struct Clthread_arg {
    int port;
}Clthread_arg;
// Function to send feedback to clients
void send_feedback_to_client(int client_sock, const char* feedback) {
    send(client_sock, feedback, strlen(feedback), 0);
}


