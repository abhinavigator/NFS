#include "headers.h"


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
    int next_port = NM_PORT + 1;
    while (1) {
        printf("Waiting for connections...\n");
        
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept");
            continue;
        }

        commstruct init_packet;
        recv(new_socket, &init_packet, sizeof(init_packet), 0);     
        // Handle the new connection
        handle_new_connection(new_socket, &init_packet);
        
        // Close the socket
        close(new_socket);
    }
    
    // Close the server socket before exiting
    close(server_fd);
    
    return 0;
}