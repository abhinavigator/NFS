#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define MAX_PATH_SIZE 50
#define MOD (int)(1e5 + 3)      // prime larger than 1e5
#define HASH_PRIME (int)31

typedef struct pathAssigned {
    char path[MAX_PATH_SIZE];
    int storage_server;
    struct pathAssigned *next;
}pathAssigned;

typedef struct pathToSS {       // hashtable mapping between path and Storage Server
    pathAssigned* hashTable[MOD];
}pathToSS;

int hash(char* path)
{
    int val = 0;
    int mul = HASH_PRIME;
    for (int i = 0; i < MAX_PATH_SIZE; i++) {
        if (path[i] == '\n') break;             // Assuming string terminates with /n (change later if necessary)
        val += mul * (int)path[i];
        val %= MOD;
        mul *= HASH_PRIME; mul%= MOD;
    }
    return val;
}

void pathToSS_init (pathToSS* HT)
{
    for (int i = 0; i < MOD; i++) {
        HT->hashTable[i] = (pathAssigned*)malloc(sizeof(pathAssigned));
        HT->hashTable[i]->next = NULL;
    }
    return;
}
 void pathToSS_node_init (pathAssigned* prev, char* path, int SS)
 {
    prev->next = (pathAssigned*)malloc(sizeof(pathAssigned));
    strcpy(prev->next->path, path);
    prev->next->storage_server = SS;
    prev->next->next = NULL;
    return;
 }

int searchPathToSS (pathToSS* HT, char* path)      // return SS of path and -1 if not found
{
    int val = hash(path);
    int flag = -1;
    pathAssigned* it = HT->hashTable[val]->next;
    while (it != NULL) {
        if (strcmp(it->path,path) == 0) {flag = it->storage_server; break;}
        it = it->next;
    }
    return flag;                    
}

void insertPathToSS (pathToSS* HT, char* path, int SS)
{
    if (searchPathToSS(HT, path) >= 0) return;
    int val = hash(path);
    pathAssigned* it = HT->hashTable[val];
    pathAssigned* temp = it->next;
    pathToSS_node_init (it, path, SS);
    it->next->next = temp;
    return;
}

void removePathToSS (pathToSS* HT, char* path, int SS)
{
    int val = hash(path);
    pathAssigned* it = HT->hashTable[val];
    int flag = 0;
    while (it->next != NULL) {
        if (strcmp(it->next->path, path) == 0) {
            pathAssigned* temp = it->next;
            it->next = temp->next;
            free(temp);
            flag = 1;
            break;
        }
    }
    if (!flag) perror("File to be deleted didn't exist\n");
    return;
}