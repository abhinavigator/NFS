#include "headers.h"

#define NM_PORT 10000
#define BUFFER_SIZE 1024
#define PATH_LENGTH 1024

// Linked list node to store Storage Server information
typedef struct StorageServerNode {
    char ip_address[INET_ADDRSTRLEN];
    int nm_port;
    int client_port;
    int ss_port;
    char path[BUFFER_SIZE][PATH_LENGTH];
    struct StorageServerNode *next;
    pthread_t SSthread;
    sem_t SSsem;
} StorageServerNode;

extern StorageServerNode *head ;


typedef struct SSthread_arg {
    int a;                      //  TO BE COMPLETED
}SSthread_arg;
// Function to send feedback to clients
void send_feedback_to_client(int client_sock, const char* feedback) {
    send(client_sock, feedback, strlen(feedback), 0);
}

// Function to add a Storage Server to the list
StorageServerNode* add_storage_server(char* ip, int nm_port, int client_port, char* path) {
    StorageServerNode *new_node = (StorageServerNode *)malloc(sizeof(StorageServerNode));
    if (new_node) {
        
        strcpy(new_node->ip_address, ip);
        new_node->nm_port = nm_port;
        new_node->client_port = client_port; 
        strcpy(new_node->path[0], path);
        new_node->next = head;
        head = new_node;
        return new_node;
    }
}
