#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define NM_PORT 10000 // Naming Server Port
#define SS_PORT 12345 // Storage Server Port (change this to your actual storage server port)

// Define the structure for communication
typedef struct {
    int type;
    char path[100];
    char data[10][100]; // Assuming an array of strings as data
} commstruct;

// Function to perform the command specified in data for the given path
void perform_NM_Command(char *path, char *command) {
    // Implement your logic here to execute the command on the Storage Server
    // Example: Perform actions based on the received command and path
    // Example: Execute the command to manipulate files or data
    printf("Performing command '%s' on path '%s'...\n", command, path);
    // Simulate completion of the task
    // You should implement the actual logic here
    printf("Task completed!\n");
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Address setup
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SS_PORT); // Using the SS_PORT

    // Bind the socket to localhost and specified port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", SS_PORT);

    while (1) {
        // Accept incoming connections
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Receive data/command from the client
        commstruct receivedCommand;

        // For simplicity, let's assume receivedCommand is properly received
        if (recv(new_socket, &receivedCommand, sizeof(receivedCommand), 0) < 0) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }

        if (receivedCommand.type == 1) { // Assuming type 1 corresponds to NM command
            printf("Received command from NM! \n");
            // Perform the command specified in data for the given path
            perform_NM_Command(receivedCommand.path, receivedCommand.data[0]);

            // Sending acknowledgment back to Naming Server
            char ack[] = "ACK"; // Dummy acknowledgment
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
