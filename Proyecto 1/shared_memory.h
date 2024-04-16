#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>

// Structs
typedef struct {
    int bufferSize;
    int writeIndex, readIndex;
    int clientBlocked, recBlocked;
    int charsTransferred, charsRemaining;
    int memUsed;
    int clientUserTime, clientKernelTime;
    int recUserTime, recKernelTime;
} SharedData;

#define MAX_TIME_LENGTH 21

typedef struct {
    char character;
    char time[MAX_TIME_LENGTH];
} Sentence;

// Funciones
void init_mem_block(char *struct_location, char *buffer_location, int sizeStruct, int sizeBuffer);

SharedData * attach_struct(char *struct_location, int size);
Sentence * attach_buffer(char *buffer_location, int size);

bool detach_struct(SharedData *sharedStruct);
bool detach_buffer(Sentence *buffer);

bool destroy_memory_block(char *filename);

// Variables
#define STRUCT_LOCATION "creador.c"
#define BUFFER_LOCATION "buffer"

#define SEM_CREATOR_FNAME "/mycreador"
#define SEM_CLIENT_FNAME "/mycliente"
#define SEM_RECONSTRUCTOR_FNAME "/myreconstructor"

#endif