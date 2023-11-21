    #include "headers.h"
    #include "storage_server.h"
     
    pthread_t client_thread;
     
    typedef struct thread_arg
    {
        int port;
        OperationType operation;
        int nm_sock;
    } thread_arg;
     
    int Writefunctionality(commstruct *C)
    {
        FILE *fptr = fopen(C->path, "w");
        if (fptr < 0)
        {
            printf("Error in opening the file provided\n");
            return 1;
        }
        fprintf(fptr, "%s", C->data[0]);
        fclose(fptr);
        printf("Data successfully written\n");
        return 0;
    }
     
    int GetSPfunctionality(commstruct *C)
    {
        FILE *fptr = fopen(C->path, "r");
        if (fptr < 0)
        {
            printf("Error in opening the file provided\n");
            return 1;
        }
        fclose(fptr);
        struct stat fileStat;
        if (stat(C->path, &fileStat) == 0)
        {
            sprintf(C->data[0], "Size: %ld bytes\nPermissions: %o\n", fileStat.st_size, fileStat.st_mode & 0777);
            printf("Command executed successfully\n");
        }
        else
        {
            return 1;
        }
        // printf("Data successfully written\n");
        return 0;
    }
     
    int Readfunctionality(commstruct *C, int new_socket)
    {
        FILE *fptr = fopen(C->path, "r");
        if (fptr < 0)
        {
            printf("Error in opening the file provided\n");
            return 1;
        }
        fseek(fptr, 0, SEEK_END);
        int fileSize = ftell(fptr);
        fseek(fptr, 0, SEEK_SET);
        int iterations = fileSize / 99;
        if (fileSize % 99)
            iterations++;
        C->filesize = iterations;
        int s = send(new_socket, C, sizeof(commstruct), 0);
        if (s < 0)
        {
            printf("Error sending size of file\n");
            return 1;
        }
        size_t bytesRead;
        while ((bytesRead = fread(C->data[0], 1, 99, fptr)) > 0)
        {
            C->data[0][bytesRead] = '\0'; // Null-terminate the buffer
            printf("%s", C->data[0]);
            int s = send(new_socket, C, sizeof(commstruct), 0);
            if (s < 0)
                printf("Error sending data of file\n");
        }
        fclose(fptr);
        return 0;
    }
     
    int Createfunctionality(commstruct *C)
    {
        if (1)
        {
            pid_t pid = fork();
            if (pid == -1)
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }
     
            if (pid == 0)
            {
                // Child process
                if (chdir(C->path) < 0)
                    printf("Error changing directory\n");
                if (strcmp(C->fileflag, "d") == 0)
                {
                    char *args[] = {"mkdir", C->data[0], NULL};
                    if (execvp(args[0], args) == -1)
                    {
                        perror("execvp");
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    int fd = open(C->data[0], O_CREAT | O_WRONLY | O_TRUNC, 0666);
     
                    if (fd == -1)
                    {
                        // Handle error if open fails
                        perror("open");
                        return 1; // Exit with an error code
                    }
     
                    // Close the file descriptor
                    close(fd);
                    exit(0);
                }
            }
            else
            {
                int status;
                waitpid(pid, &status, 0);
                printf("File/Directory created successfully\n");
                // Insert into hashtable
                // To be done immediately
            }
        }
    }
     
    int Deletefunctionality(commstruct *C)
    {
        printf("In delete\n");
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0)
        {
            // Child process
            if(1)
            {
                char *argv[] = {"rm", "-rf", C->path, NULL};
                printf("%s\n",C->path);
                // Execute the command using execvp
                if (execvp("rm", argv) == -1) {
                    perror("execvp");
                    return EXIT_FAILURE;
                }
            }
        }
        else
        {
            int status;
            waitpid(pid, &status, 0);
            printf("File/Directory deleted successfully\n");
            // Delete from hashtable
            // To be done immediately
        }
        
    }
     
    void *thread_func(void *SS_thread_arg)
    {
        thread_arg *arg = (thread_arg *)SS_thread_arg;
        int nm_ss_port = arg->port;
        int server_fd, server_fd1, new_socket;
        struct sockaddr_in address, claddr;
        socklen_t addrlen = sizeof(address);
        // Creating socket file descriptor
        if (arg->operation == Read || arg->operation == Write || arg->operation == Getsp)
        {
            if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
            {
                perror("socket failed");
                exit(EXIT_FAILURE);
            }
     
            // Forcefully attaching socket to the port 12345
            // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            //     perror("setsockopt");
            //     exit(EXIT_FAILURE);
            // }
     
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = inet_addr(IP);
            address.sin_port = htons(arg->port);
            printf("%d\n", arg->port);
     
            // Bind the socket to the address and port
            if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
            {
                perror("bind failed");
                exit(EXIT_FAILURE);
            }
            printf("bind done\n");
            // Listen for incoming connections
            if (listen(server_fd, 10) < 0)
            { // 10 is the maximum size of the pending connections queue
                perror("listen");
                exit(EXIT_FAILURE);
            }
            while (1)
            {
                new_socket = accept(server_fd, (struct sockaddr *)&claddr, (socklen_t *)&addrlen);
                if (new_socket < 0)
                {
                    perror("accept");
                    // continue;
                }
                printf("hi\n");
                commstruct *send_packet = commstruct_init(), *recv_packet = commstruct_init();
     
                // usleep(1000);
                if (recv(new_socket, recv_packet, sizeof(commstruct), 0) <= 0)
                {
                    printf("ERROR in receiving struct\n");
                    // Send acknowledgement if required over here
                    continue;
                }
                printf("Done!!!\n");
                // comparing teh operation with the available operations
     
                if (recv_packet->operation == Write)
                {
                    if (Writefunctionality(recv_packet) == 0)
                    {
                        commstruct *C = commstruct_init();
                        C->ack = 1;
                        int s = send(new_socket, recv_packet, sizeof(commstruct), 0);
                        if (s < 0)
                        {
                            printf("Error sending struct to client\n");
                        }
                        else
                        {
                            "Works\n";
                        }
                    }
                    else
                    {
                        printf("Error-File path doesnt exist\n");
                    }
                }
                else if (recv_packet->operation == Getsp)
                {
                    if (GetSPfunctionality(recv_packet) == 0)
                    {
                        printf("Works\n");
                        recv_packet->ack = 1;
                        int s = send(new_socket, recv_packet, sizeof(commstruct), 0);
                        if (s < 0)
                        {
                            printf("Error sending struct to client\n");
                        }
                        else
                        {
                            "Works\n";
                        }
                    }
                    else
                    {
                        printf("Error in executing command\n");
                    }
                }
                else if (recv_packet->operation == Read)
                {
                    if (Readfunctionality(recv_packet, new_socket) == 0)
                    {
                        commstruct *C = commstruct_init();
                        C->ack = 1;
                        int s = send(new_socket, recv_packet, sizeof(commstruct), 0);
                        if (s < 0)
                        {
                            printf("Error sending struct to client\n");
                        }
                        else
                        {
                            "Works\n";
                        }
                    }
                    else
                    {
                        printf("Error-File path doesnt exist\n");
                    }
                }
                break;
            }
        }
        else
        {
            printf("Create\n");
            int sock = arg->nm_sock;
            commstruct *send_packet = commstruct_init(), *recv_packet = commstruct_init();
            recv(sock, recv_packet, sizeof(commstruct), 0);
            printf("Op: %d", recv_packet->operation);
            if (recv_packet->operation == Delete)
            {
                Deletefunctionality(recv_packet);
                printf("Deletion completed\n");
            }
            else if (recv_packet->operation == Create)
            {
                printf("Create inside\n");
                Createfunctionality(recv_packet);
            }
            else if (recv_packet->operation == Copy)
            {
            }
            send_packet->ack = 1;
            send(sock, send_packet, sizeof(send_packet), 0);
        }
    }
    int main()
    {
     
        // Register with the Naming Server
     
        // register_with_naming_server();
     
        int sock;
        struct sockaddr_in addr;
        socklen_t addr_size;
        int n;
     
        // Create socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            perror("[-]Socket error");
            exit(1);
        }
        printf("[+]TCP server socket created.\n");
     
        // Set up server address
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(NM_PORT); // Use htons to convert to network byte order
        addr.sin_addr.s_addr = inet_addr(IP);
        inet_pton(AF_INET, IP, &addr.sin_addr);
        // Connect to the server
        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("[-]Connection error");
            close(sock); // Close the socket
            exit(1);
        }
        printf("Connected to the server.\n");
     
        int num_paths = 0;
        commstruct *sendstr = commstruct_init(), *recvstr = commstruct_init();
        sendstr->type = SS;
        printf("No of paths: ");
        scanf("%d", &sendstr->num_args);
        // sendstr->num_args = 1;
        num_paths = sendstr->num_args;
        sendstr->port = num_paths;
        strcpy(sendstr->ip, "1\n");
     
        printf("Enter Paths\n");
        for (int i = 0; i < num_paths; i++)
        {
            // scanf("%s",sendstr->ip);
            scanf("%s", sendstr->data[i]);
        }
        printf("%d\n", sendstr->num_args);
     
        // scanf("%s", buffer);
        // if (send(sock,buffer,strlen(buffer)+1,0));
        if (send(sock, sendstr, sizeof(commstruct), 0) < 0)
        {
            perror("[-]Send error");
            close(sock); // Close the socket
            exit(1);
        }
        printf("%d\n", sendstr->num_args);
     
        recv(sock, recvstr, sizeof(commstruct), 0);
        close(sock);
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            perror("[-]Socket error");
            exit(1);
        }
        printf("[+]TCP server socket created.\n");
     
        // Set up server address
        addr.sin_family = AF_INET;
        addr.sin_port = htons(recvstr->port); // Use htons to convert to network byte order
        addr.sin_addr.s_addr = inet_addr(IP);
        printf("%d\n", recvstr->port);
        inet_pton(AF_INET, IP, &addr.sin_addr);
        usleep(1000);
        // Connect to the server
        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("[-]Connection error");
            close(sock); // Close the socket
            exit(1);
        }
        // --------------------------------------------------------------------
        // --------------------------------------------------------------------
        int server_fd1, new_socket1;
        struct sockaddr_in address1;
        socklen_t addrlen1 = sizeof(address1);
        struct sockaddr_in claddr1;
        // Creating socket file descriptor
        if ((server_fd1 = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }
     
        // Forcefully attaching socket to the port 12345
        // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        //     perror("setsockopt");
        //     exit(EXIT_FAILURE);
        // }
     
        address1.sin_family = AF_INET;
        address1.sin_addr.s_addr = inet_addr(IP);
        address1.sin_port = htons(recvstr->port2);
        printf("%d\n", recvstr->port2);
     
        // Bind the socket to the address and port
        if (bind(server_fd1, (struct sockaddr *)&address1, sizeof(address1)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        printf("bind done\n");
        // Listen for incoming connections
        if (listen(server_fd1, 10) < 0)
        { // 10 is the maximum size of the pending connections queue
            perror("listen");
            exit(EXIT_FAILURE);
        }
        while (1)
        {
            int new_socket;
            new_socket = accept(server_fd1, (struct sockaddr *)&claddr1, (socklen_t *)&addrlen1);
            commstruct *send_packet = commstruct_init(), *recv_packet = commstruct_init();
     
            if (new_socket < 0)
            {
                perror("accept");
                continue;
            }
            recv(new_socket, recv_packet, sizeof(commstruct), 0);
            printf("%d*()\n", recv_packet->port);
            thread_arg *ss_thread_arg = (thread_arg *)malloc(sizeof(thread_arg));
            ss_thread_arg->nm_sock = new_socket;
            ss_thread_arg->port = recv_packet->port;
            ss_thread_arg->operation = recv_packet->operation;
            pthread_create(&client_thread, 0, thread_func, ss_thread_arg);
        }
        close(sock);
        return 0;
    }