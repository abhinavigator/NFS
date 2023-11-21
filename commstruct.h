#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#define MAX_ARGS 10
#define MAX_ARGS_LENGTH 100
#define MAX_PATH_SIZE 100

typedef enum ServerType {
    NM,
    SS,
    Client
} ServerType;

typedef enum OperationType {
    Read,
    Write,
    Delete,
    Copy,
    Getsp,
    Create
} OperationType;

typedef struct commstruct {
    ServerType type;                  // 0 for NM, 1 for SS, 2 for client
    OperationType operation;
    char data[MAX_ARGS][MAX_ARGS_LENGTH];
    char path[MAX_PATH_SIZE];
    char ip[INET_ADDRSTRLEN];
    int port;
    int port2;
    int num_args;
    int filesize;
    int ack;
    char fileflag[2];
}commstruct;

commstruct* commstruct_init ()
{
    commstruct* new = (commstruct*)malloc(sizeof(commstruct));
    new->port = 0;
    new->type = -1;
    new->num_args = 0;
    new->ack=0;
    new->operation = -1;
    new->filesize=0;
    return new;
}
