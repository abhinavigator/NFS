#include "common.h"
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
    sendstr->type = Client;
    // sendstr->num_args = 1;

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
    printf("%d--\n", recvstr->port);
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
    printf("%d\n",recvstr->port);
    inet_pton(AF_INET,IP,&addr.sin_addr);
    usleep(1000);
    // Connect to the server
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("[-]Connection error");
        close(sock); // Close the socket
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
            scanf("%s", sendstr->data[0]);
        }
        else if (strcmp(oper, "r") == 0)
        {
            sendstr->operation = Read;
            printf("File to read:");
            scanf("%s", sendstr->data[0]);
        }
        else if (strcmp(oper, "d") == 0)
        {
            sendstr->operation = Delete;
            printf("Delete what:");
            scanf("%s", sendstr->data[0]);
        }
        else if (strcmp(oper, "c") == 0)
        {
            sendstr->operation = Copy;
            printf("Copy what");
            scanf("%s", sendstr->data[0]);
        }
        else if (strcmp(oper, "g") == 0)
        {
            sendstr->operation = Getsp;
            printf("Getsp of what:");
            scanf("%s", sendstr->data[0]);
        }
        else if (strcmp(oper, "n") == 0)
        {
            sendstr->operation = Create;
            printf("create Where:");
            scanf("%s", sendstr->data[0]);
            printf("Create What");
            scanf("%s", sendstr->data[1]);
        }
    }
}