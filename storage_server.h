#ifndef SS_H
#define SS_H

#include "headers.h"


#define NM_PORT 12345
#define NM_IP "127.0.0.1"
#define CLIENT_PORT 54321 // Different for each SS
#define BUFFER_SIZE 1024

// void register_with_naming_server(int sock, const char* ip, int client_port, const char* paths);
// void handle_nm_commands(int nm_sock);
// void handle_client_requests(int client_sock);

// Function definitions
void register_with_naming_server(int sock, const char* ip, int client_port, const char* paths) {
    // Create a registration message with the format "REGISTER,IP,client_port,paths"
    char registration_message[BUFFER_SIZE];

    // Previous code line
    // snprintf(registration_message, sizeof(registration_message), "REGISTER,%s,%d,%s", ip, client_port, paths);

    //  Added NM_PORT to the message
    snprintf(registration_message, sizeof(registration_message), "REGISTER,%s,%d,%d,%s", ip, NM_PORT, client_port, paths);

    
    // Send the registration message to the Naming Server
    if (send(sock, registration_message, strlen(registration_message), 0) < 0) {
        perror("send failed");
        exit(EXIT_FAILURE); // Exit if sending failed
    }
}

void handle_nm_commands(int nm_sock) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = recv(nm_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the string
        // Example command format: "COMMAND,parameter1,parameter2,..."
        char* command = strtok(buffer, ",");
        
        if (strcmp(command, "CREATE") == 0) {
            char* path = strtok(NULL, ",");
            char* type = strtok(NULL, ",");
            if (strcmp(type, "FILE") == 0) {
                create_file(path);
            } else if (strcmp(type, "DIR") == 0) {
                create_directory(path);
            }
        } else if (strcmp(command, "DELETE") == 0) {
            char* path = strtok(NULL, ",");
            delete_path(path); // This function will handle both files and directories
        } else if (strcmp(command, "COPY") == 0) {
            char* source_ip = strtok(NULL, ",");
            char* source_path = strtok(NULL, ",");
            char* destination_path = strtok(NULL, ",");
            copy_file_or_directory(source_ip, source_path, destination_path);
        }
        // Other commands can be added here
    }
}

void handle_client_requests(int client_sock) {
    // Implementation to handle client requests
    char buffer[1024];
    ssize_t bytes_read = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the string
        // parse the buffer for requests like READ, WRITE, GET_INFO
        // For example, if the request is READ, call a read_file function
        char* command = strtok(buffer, ",");
        if (strcmp(command, "READ") == 0) {
            char* filename = strtok(NULL, ",");
            read_file(client_sock, filename);
        } else if (strcmp(command, "WRITE") == 0) {
            char* filename = strtok(NULL, ",");
            char* content = strtok(NULL, ""); // Read the rest of the buffer as content
            write_to_file(filename, content);
        } else if (strcmp(command, "INFO") == 0) {
            char* filename = strtok(NULL, ",");
            get_file_info(client_sock, filename);
        }
    }
}



#endif