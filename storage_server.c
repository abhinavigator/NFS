#include "headers.h"
#include "storage_server.h"



int main() {


    // Register with the Naming Server
   
    register_with_naming_server();

   

    // Create socket to listen for client requests
    // int client_listener = socket(AF_INET, SOCK_STREAM, 0);
    // // [Bind and listen on client_listener]
    
    // // Set the socket to non-blocking mode
    // fcntl(client_listener, F_SETFL, O_NONBLOCK);
    
    // // Main loop to handle Naming Server commands and client requests
    // while (1) {
    //     // Handle commands from Naming Server
    //     // handle_nm_commands(sock);
        
    //     // Accept new client connections and handle their requests
    //     struct sockaddr_in client_addr;
    //     socklen_t client_addr_len = sizeof(client_addr);
    //     int client_sock = accept(client_listener, (struct sockaddr *)&client_addr, &client_addr_len);
    //     if (client_sock > 0) {
    //         // Received a client connection
    //         handle_client_requests(client_sock);
    //         close(client_sock);
    //     }
        
    //     // Sleep to prevent this loop from consuming too much CPU
    //     usleep(100000); // Sleep for 100 ms
    // }

    // // Close the sockets
    
    // close(client_listener);
    
    return 0;
}

