#include "headers.h"


int main() {
    struct sockaddr_in serv_addr;
    int sock = 0;
    char details[BUFFER_SIZE];

    // Create socket and connect to Naming Server
    // [Socket creation and connection logic here]

    // Create socket and connect to Naming Server
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(NM_PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, NM_IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    // Register with the Naming Server
   
    register_with_naming_server(sock, "Storage Server IP", CLIENT_PORT, "path1;path2;pathN");

   

    // Create socket to listen for client requests
    int client_listener = socket(AF_INET, SOCK_STREAM, 0);
    // [Bind and listen on client_listener]
    
    // Set the socket to non-blocking mode
    fcntl(client_listener, F_SETFL, O_NONBLOCK);
    
    // Main loop to handle Naming Server commands and client requests
    while (1) {
        // Handle commands from Naming Server
        handle_nm_commands(sock);
        
        // Accept new client connections and handle their requests
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_sock = accept(client_listener, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sock > 0) {
            // Received a client connection
            handle_client_requests(client_sock);
            close(client_sock);
        }
        
        // Sleep to prevent this loop from consuming too much CPU
        usleep(100000); // Sleep for 100 ms
    }

    // Close the sockets
    close(sock);
    close(client_listener);
    
    return 0;
}

