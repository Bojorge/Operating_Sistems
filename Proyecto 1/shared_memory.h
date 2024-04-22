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
    int readingFileIndex;
    int clientBlockedTime, reconsBlockedTime;
    int charsTransferred, charsInBuffer;
    int memUsed;
    int clientUserTime, clientKernelTime;
    int recUserTime, recKernelTime;
    bool writingFinished, statsInited;
    bool clientEndedProcess, reconsEndedProcess;
} SharedData;

#define SEM_W_INDEX "/writeIndexSem"
#define SEM_R_INDEX "/readIndexSem"
#define SEM_RF_INDEX "/readFileIndexSem"
#define SEM_CB_TIME "/clientBlockTimeSem"
#define SEM_RB_TIME "/reconsBlockTimeSem"
#define SEM_C_TRANSF "/charsTransfSem"
#define SEM_C_BUFFER "/charsBufferSem"

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
#define BUFFER_LOCATION "destroy.c"

#define SEM_CLIENT_PROCESS_FNAME "/myprocessclient"
#define SEM_RECONS_PROCESS_FNAME "/myprocessrecons"
#define SEM_READ_VARIABLE_FNAME "/mybufferreadvariable"
#define SEM_WRITE_VARIABLE_FNAME "/mybufferwritevariable"

#define MAX_LENGTH 100

#endif