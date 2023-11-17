#include "headers.h"

void create_file(const char* path) {
    // Implementation to create a file
    FILE* file = fopen(path, "w");
    if (file) {
        // File created successfully.
        fclose(file);
    } else {
        // Handle error.
    }
}

void read_file(int client_sock, const char* path) {
    // Implementation to read a file
    char buffer[1024];
    FILE* file = fopen(path, "r");
    if (file) {
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            send(client_sock, buffer, bytes_read, 0);
        }
        fclose(file);
    } else {
        // Handle error.
    }
}

void create_directory(const char* path) {
    mkdir(path, 0777); // Use appropriate permissions
    // Handle directory creation error
}

void delete_path(const char* path) {
    // Determine if path is a file or a directory
    struct stat path_stat;
    stat(path, &path_stat);
    if (S_ISDIR(path_stat.st_mode)) {
        // It's a directory
        rmdir(path); // Remove directory (only works if directory is empty)
    } else {
        // It's a file
        remove(path); // Delete file
    }
    // Handle deletion error
}



void copy_file_or_directory(const char* source_ip, const char* source_path, const char* destination_path) {
    // Implement logic to copy file or directory from another SS
    // This could involve setting up a client connection to the source SS and requesting the file data
    // Then, the data would be written to the destination path on the local SS
    copy_file_from_ss(source_ip, NM_PORT, source_path, destination_path);
}




// Server-side: Function to serve a file to another SS
void serve_file_to_ss(int server_sock, const char* file_path) {
    send_file(server_sock, file_path);
    int file_fd = open(file_path, O_RDONLY);
    if (file_fd < 0) {
        perror("Failed to open file to serve");
        return;
    }

    struct stat file_stat;
    if (fstat(file_fd, &file_stat) < 0) {
        perror("Failed to get file stats");
        close(file_fd);
        return;
    }

    off_t offset = 0;
    // send_file(server_sock, file_fd, &offset, file_stat.st_size);
    close(file_fd);
}

// Client-side: Function to copy a file from another SS
void copy_file_from_ss(const char* source_ip, int source_port, const char* source_file_path, const char* destination_file_path) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation error");
        return;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(source_port);

    if (inet_pton(AF_INET, source_ip, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sock);
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        close(sock);
        return;
    }

    // Send a request for the file
    // For simplicity, we assume the protocol is just to send the file path
    send(sock, source_file_path, strlen(source_file_path), 0);

    // Open the file for writing
    int file_fd = open(destination_file_path, O_WRONLY | O_CREAT, 0666);
    if (file_fd < 0) {
        perror("Failed to open file for writing");
        close(sock);
        return;
    }

    // Receive the file
    char buffer[4096];
    ssize_t bytes_received;
    while ((bytes_received = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        write(file_fd, buffer, bytes_received);
    }

    close(file_fd);
    close(sock);
}


// Function to send a file over a socket without using sendfile
void send_file(int out_fd, const char* file_path) {
    int in_fd = open(file_path, O_RDONLY);
    if (in_fd < 0) {
        perror("Failed to open file for reading");
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_sent, bytes_to_send;

    while ((bytes_read = read(in_fd, buffer, BUFFER_SIZE)) > 0) {
        char *ptr = buffer;
        bytes_to_send = bytes_read;
        while (bytes_to_send > 0) {
            bytes_sent = send(out_fd, ptr, bytes_to_send, 0);
            if (bytes_sent < 0) {
                perror("Failed to send file data");
                close(in_fd);
                return;
            }
            ptr += bytes_sent;
            bytes_to_send -= bytes_sent;
        }
    }

    if (bytes_read < 0) {
        perror("Failed to read from file");
    }

    close(in_fd);
}

void write_to_file(const char* filename, const char* content) {
    // Assumes content is a null-terminated string
    FILE* file = fopen(filename, "w");
    if (file) {
        fputs(content, file); // Write content to file
        fclose(file);
    } else {
        // Handle error in file opening
    }
}

void get_file_info(int client_sock, const char* filename) {
    struct stat statbuf;
    if (stat(filename, &statbuf) == 0) {
        char info[BUFFER_SIZE];
        // Format the size and permissions into the info string
        snprintf(info, sizeof(info), "Size: %ld, Permissions: %o",
                 (long)statbuf.st_size, statbuf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
        // Send the info back to the client
        send(client_sock, info, strlen(info), 0);
    } else {
        // Handle error in stat
    }
}


// [Remaining code...]

