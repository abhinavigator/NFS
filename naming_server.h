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
} StorageServerNode;

extern StorageServerNode *head ;
int ss_count;
// Head of the linked list for Storage Servers



// Function to send feedback to clients
void send_feedback_to_client(int client_sock, const char* feedback) {
    send(client_sock, feedback, strlen(feedback), 0);
}

// Function to handle new connections, assume it can differentiate between SS and client
void handle_new_connection(int new_socket, commstruct* init_packet) {
    char buffer[BUFFER_SIZE] = {0};
    
    // Read details from the connection
    read(new_socket, buffer, BUFFER_SIZE);
    
    // Check if it's from SS or client based on the data format
    // This is a simplified example, in a real scenario you would use a more complex check
    if (strstr(buffer, "REGISTER")) {
        // Parse the details for the Storage Server and add to the list
        // Assuming the format is "REGISTER,IP,nm_port,client_port,path1;path2;...;pathN"
        char* token = strtok(buffer, ",");
        token = strtok(NULL, ","); // Skip "REGISTER"
        char* ip = token;
        
        token = strtok(NULL, ",");
        int nm_port = atoi(token);
        
        token = strtok(NULL, ",");
        int client_port = atoi(token);
        
        token = strtok(NULL, ",");
        char* paths = token;
        
        add_storage_server(ip, nm_port, client_port, paths);
    } else {
        // Assume it's a client request and provide feedback
        send_feedback_to_client(new_socket, "Task completed successfully");
    }
}
