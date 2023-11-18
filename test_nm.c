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

int main() {
    int sock;
    struct sockaddr_in server_addr;
    commstruct send_struct;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SS_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), '\0', 8);

    // Connect to the Storage Server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the structure to send
    send_struct.type = 1; // Example command type
    strcpy(send_struct.path, "/path/to/file.txt"); // Example path
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

    return 0;
}
