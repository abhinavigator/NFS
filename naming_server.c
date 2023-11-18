#include "headers.h"


StorageServerNode *head = NULL;

// head = NULL;



int ss_count = 0;
int next_port = NM_PORT + 1;
pathToSS* Hashtable;

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

void* SSfunc(void* SS_thread_arg)                      // TO BE COMPLETED
{
    return NULL;
}
void find_new_port()
{
    int sockfd;
    struct sockaddr_in server_addr;
    while (true) {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        }
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(next_port);
        if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0) {
            close(sockfd);
            exit(EXIT_SUCCESS);
        }
        else next_port++;

    }
}
// Function to handle new connections, assume it can differentiate between SS and client
void handle_new_connection(int new_socket, commstruct* init_packet) {
    if (init_packet->type == SS) {
        find_new_port();

        commstruct* send_packet  = commstruct_init();
        for (int i = 0; i < init_packet->num_args; i++) {
            insertPathToSS(Hashtable, init_packet->data[i], ss_count);
        }
        StorageServerNode* newNode = add_storage_server("", NM_PORT, 1, init_packet->path);
        SSthread_arg* ss_thread_arg = (SSthread_arg*)malloc(sizeof(SSthread_arg));
        pthread_create(&(newNode->SSthread), 0, SSfunc, ss_thread_arg);
        ss_count++;
        send_packet->port = next_port;
    
        send(new_socket, init_packet, sizeof(commstruct), 0 );
    }
    return;
}

void send_command_to_SS()
{
      int sock;
    struct sockaddr_in server_addr;
    commstruct send_struct;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), '\0', 8);

    // Connect to the Storage Server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the structure to send
    send_struct.type = 1; // Example command type
    strcpy(send_struct.data[1], "dir1/file.txt"); // Example path
    printf("-->%s\n",send_struct.path);
    strcpy(send_struct.data[0], "create_file"); // Example command

    // Send the structure to the Storage Server
    if (send(sock, &send_struct, sizeof(send_struct), 0) == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

    printf("Command sent to Storage Server: %s\n", send_struct.data[0]);

    // Receive acknowledgment from Storage Server
    char ack[4];
    if (recv(sock, ack, sizeof(ack), 0) == -1) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    }

    // Check acknowledgment and print task completion message
    if (strcmp(ack, "ACK") == 0) {
        printf("Received acknowledgment from Storage Server\n");
        printf("Task completed!\n");
    }

    // Close the socket
    close(sock);

}


int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    Hashtable = (pathToSS*)malloc(sizeof(pathToSS));
    pathToSS_init(Hashtable);
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

        commstruct send_packet, recv_packet;;
        recv(new_socket, &recv_packet, sizeof(recv_packet), 0);     
        // Handle the new connection
        handle_new_connection(new_socket, &recv_packet);

        send_command_to_SS();
        
        // Close the socket
        close(new_socket);
    }
    
    // Close the server socket before exiting
    close(server_fd);
    
    return 0;
}