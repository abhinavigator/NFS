// #include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "headers.h"

// Function declarations

// More function implementations
int main(int argc, char *argv[])
{
    int sock_NM_C;
    struct sockaddr_in addr;
    socklen_t addr_size;
    int n;

    // Create socket
    sock_NM_C = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_NM_C < 0)
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
    if (connect(sock_NM_C, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("[-]Connection error");
        close(sock_NM_C); // Close the socket
        exit(1);
    }
    printf("Connected to the server.\n");

    int num_paths = 0;
    commstruct *sendstr = commstruct_init(), *recvstr = commstruct_init();
    sendstr->type = Client;
    // sendstr->num_args = 1;

    // scanf("%s", buffer);
    // if (send(sock,buffer,strlen(buffer)+1,0));
    if (send(sock_NM_C, sendstr, sizeof(commstruct), 0) < 0)
    {
        perror("[-]Send error");
        close(sock_NM_C); // Close the socket
        exit(1);
    }
    printf("%d\n", sendstr->num_args);

    recv(sock_NM_C, recvstr, sizeof(commstruct), 0);
    printf("%d--\n", recvstr->port);
    close(sock_NM_C);
    sock_NM_C = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_NM_C < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    // Set up server address
    int store_sock = recvstr->port;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(recvstr->port); // Use htons to convert to network byte order
    addr.sin_addr.s_addr = inet_addr(IP);
    printf("%d\n", recvstr->port);
    inet_pton(AF_INET, IP, &addr.sin_addr);
    usleep(1000);
    // Connect to the server
    if (connect(sock_NM_C, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("[-]Connection error");
        close(sock_NM_C); // Close the socket
        exit(1);
    }
    while (true)
    {
        printf("Operation (r/w/d/c(copy)/g/n(create)):");

        char oper[10];
        scanf("%s", oper);
        if (strcmp(oper, "w") == 0)
        {
            sendstr->operation = Write;
            printf("File to Write:");
            scanf("%s", sendstr->path);
            printf("What to write: ");
            scanf("%s", sendstr->data[0]);
        }
        else if (strcmp(oper, "r") == 0)
        {
            sendstr->operation = Read;
            printf("File to read:");
            scanf("%s", sendstr->path);
        }
        else if (strcmp(oper, "d") == 0)
        {
            sendstr->operation = Delete;
            printf("Delete what:");
            scanf("%s", sendstr->path);
        }
        else if (strcmp(oper, "c") == 0)
        {
            sendstr->operation = Copy;
            printf("Copy what");
            scanf("%s", sendstr->path);
        }
        else if (strcmp(oper, "g") == 0)
        {
            sendstr->operation = Getsp;
            printf("Getsp of what:");
            scanf("%s", sendstr->path);
        }
        else if (strcmp(oper, "n") == 0)
        {
            sendstr->operation = Create;
            printf("Create Where:");
            scanf("%s", sendstr->path);
            printf("Create What:");
            scanf("%s", sendstr->data[0]);
            printf("File(f) or Directory(d):");
            scanf("%s",(sendstr->fileflag));
        }
        else
        {
            printf("Illegal command\n");
            continue;
        }
        printf("a\n");
        if (send(sock_NM_C, sendstr, sizeof(commstruct), 0) < 0)
        {
            printf("here\n");
            perror("[-]Send error");
            close(sock_NM_C); // Close the socket
            exit(1);
        }
        printf("b\n");
        int sock_SS_C = socket(AF_INET, SOCK_STREAM, 0);

        if (sendstr->operation == Read || sendstr->operation == Write || sendstr->operation == Getsp)
        {
            recv(sock_NM_C, recvstr, sizeof(commstruct), 0);
            printf("c\n");
            // close(sock);
            if (sock_SS_C < 0)
            {
                perror("[-]Socket error");
                exit(1);
            }
            printf("[+]TCP server socket created.\n");

            // Set up server address
            usleep(1000);
            printf("%d\n", recvstr->port);
            int store_sock = recvstr->port;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(recvstr->port); // Use htons to convert to network byte order
            addr.sin_addr.s_addr = inet_addr(IP);
            if (connect(sock_SS_C, (struct sockaddr *)&addr, sizeof(addr)) < 0)
            {
                perror("[-]Connection error");
                close(sock_SS_C); // Close the socket
                exit(1);
            }
            if (send(sock_SS_C, sendstr, sizeof(commstruct), 0) < 0)
            {
                perror("[-]Send error");
                close(sock_SS_C); // Close the socket
                exit(1);
            }
            printf("yay\n");
            if (sendstr->operation == Read)
            {
                commstruct *size1 = commstruct_init();
                recv(sock_SS_C, size1, sizeof(commstruct), 0);
                int iterations = size1->filesize;
                for (int i = 0; i < iterations; i++)
                {
                    recv(sock_SS_C, size1, sizeof(commstruct), 0);
                    printf("%s", size1->data[0]);
                    strcpy(size1->data[0], "");
                }
                printf("\n");
            }
            commstruct *ack1 = commstruct_init();
            if (recv(sock_SS_C, ack1, sizeof(commstruct), 0) <= 0)
            {
                printf("ERROR in receiving acknowledgement struct\n");
                // Send acknowledgement if required over here
                // continue;
            }
            else
            {
                if(sendstr->operation==Getsp)
                {
                    printf("%s",ack1->data[0]); // printf the getsp
                    strcpy(ack1->data[0],"");
                }
                printf("Command successfully executed\n");
            }
            close(sock_SS_C);
        }
        else
        {
            commstruct *ack1 = commstruct_init();
            if (recv(sock_NM_C, ack1, sizeof(commstruct), 0) <= 0)
            {
                printf("ERROR in receiving acknowledgement struct\n");
                // Send acknowledgement if required over here
                // continue;
            }
            else
            {
                printf("Command successfully executed\n");
            }
        }
        
    }
    close(sock_NM_C);
}