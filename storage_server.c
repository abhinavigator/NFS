#include "headers.h"
#include "storage_server.h"

// int main() {

//     // Register with the Naming Server

//     register_with_naming_server();

//     while (1) {
//         // Code to receive 'receivedCommand' from the Naming Server
//         commstruct receivedCommand;

//         // For simplicity, let's assume receivedCommand is properly received

//         // Assuming num_args is 1 for NM server
//         if ( receivedCommand.type == NM) {
//             // Perform the command specified in data for the given path
//             perform_NM_Command(receivedCommand.path, receivedCommand.data[0]);
//         }
//         // Add conditions for other types of commands or responses
//     }

//     return 0;
// }

int main()
{

    register_with_naming_server();

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Address setup
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PORT); // Using the NM_PORT

    // Bind the socket to localhost and specified port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", SERVER_PORT);

// ... (existing code)


    while (1) {
        // Accept incoming connections
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Receive data/command from the client
        commstruct receivedCommand; // Change to a non-pointer

        // For simplicity, let's assume receivedCommand is properly received
        if (recv(new_socket, &receivedCommand, sizeof(receivedCommand), 0) < 0) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }


// On the Storage Server side after receiving the command
if (receivedCommand.type == 1) {
    printf("Received command from NM! \n");
    printf("Received Path: '%s'\n", receivedCommand.data[1]); // Check received path

    // Check the path length
    // printf("Path Length: %zu\n", strlen(receivedCommand.path));

    perform_NM_Command(receivedCommand.data[1], receivedCommand.data[0]);

    // Sending acknowledgment back to Naming Server
    char ack[] = "ACK";
    send(new_socket, ack, sizeof(ack), 0);
}

        // Add conditions for other types of commands or responses

        // Close the socket for this specific connection
        close(new_socket);
    }

    // Close the main server socket
    close(server_fd);

    return 0;
}
