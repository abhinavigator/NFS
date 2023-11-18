#include "headers.h"
#include "storage_server.h"

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
    memset(&addr,0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(NM_PORT); // Use htons to convert to network byte order
    addr.sin_addr.s_addr = inet_addr(IP);
    inet_pton(AF_INET,IP,&addr.sin_addr);
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
    printf("jsnddnsj-----5\n");
    close(sock);
    while(true){}
    return 0;
}
