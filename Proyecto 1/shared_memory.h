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

typedef struct {
    char *buffer;
    int bufferSize;
    int writeIndex, readIndex;
    int clientBlocked, recBlocked;
    int charsTransferred, charsRemaining;
    int memUsed;
    time_t *insertTimes;
    int clientUserTime, clientKernelTime;
    int recUserTime, recKernelTime;
} SharedData;

/*typedef struct {
    char *buffer;
    time_t *insertTimes;
} BufferData;*/

SharedData * init_mem_block(char *filename, int size, int numChars);
SharedData * attach_memory_block(char *filename, int size);
bool detach_memory_block(SharedData *sharedData);
bool destroy_memory_block(char *filename);

#define FILENAME "creador.c"
#define BLOCK_SIZE 73

#define SEM_CREATOR_FNAME "/mycreador"
#define SEM_CLIENT_FNAME "/mycliente"
#define SEM_RECONSTRUCTOR_FNAME "/myreconstructor"

#endif