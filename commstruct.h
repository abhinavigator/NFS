#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#define MAX_ARGS 100
#define MAX_ARGS_LENGTH 1024

typedef struct commstruct {
    int type;                   // 0 for NM, 1 for SS, 2 for client
    char data[MAX_ARGS][MAX_ARGS_LENGTH];
    char path[MAX_PATH_SIZE];
    int num_args;
}commstruct;
