#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#define MAX_ARGS 100
#define MAX_ARGS_LENGTH 1024
#define MAX_PATH_SIZE 1024

typedef enum ServerType {
    NM,
    SS,
    Client
}ServerType;

typedef struct commstruct {
    ServerType type;                  // 0 for NM, 1 for SS, 2 for client
    char data[MAX_ARGS][MAX_ARGS_LENGTH];
    char path[MAX_PATH_SIZE];
    char ip[INET_ADDRSTRLEN];
    int port;
    int num_args;
}commstruct;

commstruct* commstruct_init ()
{
    commstruct* new = (commstruct*)malloc(sizeof(commstruct));
    new->port = 0;
    new->type = -1;
    new->num_args = 0;
    return new;
}
