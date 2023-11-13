#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define NM_PORT 12345
#define BUFFER_SIZE 1024

// Linked list node to store Storage Server information
typedef struct StorageServerNode {
    char ip_address[INET_ADDRSTRLEN];
    int nm_port;
    int client_port;
    int ss_port;
    char path[BUFFER_SIZE];
    struct StorageServerNode *next;
} StorageServerNode;

// Head of the linked list for Storage Servers
StorageServerNode *head = NULL;
int ss_count = 0;

// Function to add a Storage Server to the list
void add_storage_server(const char* ip, int nm_port, int client_port, const char* path) {
    StorageServerNode *new_node = (StorageServerNode *)malloc(sizeof(StorageServerNode));
    if (new_node) {
        strcpy(new_node->ip_address, ip);
        new_node->nm_port = nm_port;
        new_node->client_port = client_port;
        strcpy(new_node->path, path);
        new_node->next = head;
        head = new_node;
        ss_count++;
    }
}

// Function to send feedback to clients
void send_feedback_to_client(int client_sock, const char* feedback) {
    send(client_sock, feedback, strlen(feedback), 0);
}

// Function to handle new connections, assume it can differentiate between SS and client
void handle_new_connection(int new_socket) {
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

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 12345
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(NM_PORT);

    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 10) < 0) { // 10 is the maximum size of the pending connections queue
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Naming Server is running on port %d\n", NM_PORT);
    
    while (1) {
        printf("Waiting for connections...\n");
        
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept");
            continue;
        }
        
        // Handle the new connection
        handle_new_connection(new_socket);
        
        // Close the socket
        close(new_socket);
    }
    
    // Close the server socket before exiting
    close(server_fd);
    
    return 0;
}