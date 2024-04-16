#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared_memory.h"

void init_empty_struct (SharedData *sharedData, int numChars) {
    sharedData->bufferSize = numChars;
    sharedData->writeIndex = 0;
    sharedData->readIndex = 0;
    sharedData->clientBlocked = 0;
    sharedData->recBlocked = 0;
    sharedData->charsTransferred = 0;
    sharedData->charsRemaining = 0;
    sharedData->clientUserTime = 0;
    sharedData->clientKernelTime = 0;
    sharedData->recUserTime = 0;
    sharedData->recKernelTime = 0;
    sharedData->memUsed = sizeof(SharedData) + (sizeof(Sentence) * numChars);
}

void printSharedData(SharedData *sharedData) {
    /*printf("sharedData->bufferSize: %d\n", sharedData->bufferSize);
    printf("sharedData->writeIndex: %d\n", sharedData->writeIndex);
    printf("sharedData->readIndex: %d\n", sharedData->readIndex);
    printf("sharedData->clientBlocked: %d\n", sharedData->clientBlocked);
    printf("sharedData->recBlocked: %d\n", sharedData->recBlocked);
    printf("sharedData->charsTransferred: %d\n", sharedData->charsTransferred);
    printf("sharedData->charsRemaining: %d\n", sharedData->charsRemaining);
    printf("sharedData->memUsed: %d\n", sharedData->memUsed);
    printf("sharedData->clientUserTime: %d\n", sharedData->clientUserTime);
    printf("sharedData->clientKernelTime: %d\n", sharedData->clientKernelTime);
    printf("sharedData->recUserTime: %d\n", sharedData->recUserTime);
    printf("sharedData->recKernelTime: %d\n", sharedData->recKernelTime);*/
}

int main(int argc, char *argv[]) 
{
    destroy_memory_block(STRUCT_LOCATION);
    destroy_memory_block(BUFFER_LOCATION);
    
    int numChars;
    printf("Ingrese la cantidad de caracteres a compartir: ");
    scanf("%d", &numChars);

    // Set the semaphores
    sem_unlink(SEM_CREATOR_FNAME);
    sem_unlink(SEM_CLIENT_FNAME);
    sem_unlink(SEM_RECONSTRUCTOR_FNAME);

    sem_t *sem_crt = sem_open(SEM_CREATOR_FNAME, O_CREAT, 0644, 1);
    if (sem_crt == SEM_FAILED) {
        perror("sem_open/creator");
        exit(EXIT_FAILURE);
    }
    sem_t *sem_clt = sem_open(SEM_CLIENT_FNAME, O_CREAT, 0644, 0);
    if (sem_clt == SEM_FAILED) {
        perror("sem_open/client");
        exit(EXIT_FAILURE);
    }
    sem_t *sem_rcstr = sem_open(SEM_RECONSTRUCTOR_FNAME, O_CREAT, 0644, 0);
    if (sem_rcstr == SEM_FAILED) {
        perror("sem_open/reconstructor");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < numChars; i++) {
        sem_unlink(SEM_VARIABLE_FNAME );
    }

    // Initialize shared mem blocks
    init_mem_block(STRUCT_LOCATION, BUFFER_LOCATION, sizeof(SharedData), numChars * sizeof(Sentence));

    // Attach to struct shared mem block
    SharedData *sharedStruct = attach_struct(STRUCT_LOCATION, sizeof(SharedData));
    if (sharedStruct == NULL) {
        printf("Error al adjuntar al bloque de memoria compartida.\n");
        exit(EXIT_FAILURE);
    }

    // Attach to buffer mem block
    Sentence *buffer = attach_buffer(BUFFER_LOCATION, numChars * sizeof(Sentence));
    if (buffer == NULL) {
        printf("Error al adjuntar al bloque de memoria compartida.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize empty struct for the shared mem block
    init_empty_struct(sharedStruct, numChars);

    // Start visualization of mem block
    while(true) {
        sem_wait(sem_clt);
        
        for (int i = 0; i < numChars; i++) {
            printf("buffer[%d] = \"%c\" | time: %s\n", i, buffer[i].character, buffer[i].time);
        }

        printf("--------------------------------------\n");

        sem_post(sem_crt);
        sem_post(sem_rcstr);
    }

    detach_struct(sharedStruct);
    detach_buffer(buffer);

    // Destroy the shared mem block and semaphores
    sem_close(sem_crt);
    sem_close(sem_clt);
    sem_close(sem_rcstr);

    destroy_memory_block(STRUCT_LOCATION);
    destroy_memory_block(BUFFER_LOCATION);

    return 0;
}