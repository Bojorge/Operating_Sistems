#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared_memory.h"

void init_empty_struct (SharedData *sharedData, int numChars) {
    char *emptyCharArray = (char *)calloc(numChars, sizeof(char));
    time_t *emptyTimeArray = (time_t *)calloc(numChars, sizeof(time_t));
    sharedData->buffer = emptyCharArray;
    sharedData->bufferSize = numChars;
    sharedData->writeIndex = 0;
    sharedData->readIndex = 0;
    sharedData->clientBlocked = 0;
    sharedData->recBlocked = 0;
    sharedData->charsTransferred = 0;
    sharedData->charsRemaining = 0;
    sharedData->insertTimes = emptyTimeArray;
    sharedData->clientUserTime = 0;
    sharedData->clientKernelTime = 0;
    sharedData->recUserTime = 0;
    sharedData->recKernelTime = 0;
    sharedData->memUsed = sizeof(sharedData);
}

void printSharedData(SharedData *sharedData) {
    /*printf("sharedData->buffer: %p\n", sharedData->buffer);
    printf("sharedData->bufferSize: %d\n", sharedData->bufferSize);
    printf("sharedData->writeIndex: %d\n", sharedData->writeIndex);
    printf("sharedData->readIndex: %d\n", sharedData->readIndex);
    printf("sharedData->clientBlocked: %d\n", sharedData->clientBlocked);
    printf("sharedData->recBlocked: %d\n", sharedData->recBlocked);
    printf("sharedData->charsTransferred: %d\n", sharedData->charsTransferred);
    printf("sharedData->charsRemaining: %d\n", sharedData->charsRemaining);
    printf("sharedData->memUsed: %d\n", sharedData->memUsed);
    printf("sharedData->insertTimes: %p\n", sharedData->insertTimes);
    printf("sharedData->clientUserTime: %d\n", sharedData->clientUserTime);
    printf("sharedData->clientKernelTime: %d\n", sharedData->clientKernelTime);
    printf("sharedData->recUserTime: %d\n", sharedData->recUserTime);
    printf("sharedData->recKernelTime: %d\n", sharedData->recKernelTime);*/

    printf("Contenido del buffer: \n");
        for (int i = 0; i < sharedData->bufferSize; i++) {
            printf("\"%c\" en posicion: %d ", sharedData->buffer[i], i);
            printf("insertado a las: %ld\n", sharedData->insertTimes[i]);
        }
}

int main(int argc, char *argv[]) 
{
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

    // Initialize shared mem block
    int sharedSize = sizeof(SharedData) + numChars * sizeof(char) + numChars * sizeof(time_t);
    printf("Tamano sharedSize: %d\n", sharedSize);

    SharedData *sharedData = init_mem_block(FILENAME, sharedSize, numChars);
    if (sharedData == NULL) {
        printf("ERROR: no se pudo crear el bloque\n");
        return -1;
    }

    // Structure the shared mem block
    init_empty_struct(sharedData, numChars);
    detach_memory_block(sharedData);

    // Start visualization of mem block
    while(true) {
        SharedData *sharedData = attach_memory_block(FILENAME, sharedSize);
        if (sharedData == NULL) {
            printf("ERROR: no se pudo crear el bloque\n");
            return -1;
        }

        sem_wait(sem_clt);
            /*for (int i = 0; i < sharedData->bufferSize; i++) {
                printf("\"%c\" en posicion: %d ", sharedData->buffer[i], i);
                printf("insertado a las: %ld\n", sharedData->insertTimes[i]);
            }*/
            /*if (strlen(sharedData) > 0) {
                printf("Reading: \"%s\"\n", sharedData);
                bool done = (strcmp(sharedData, "quit") == 0);
                sharedData[0] = 0;
                if (done) {break;}
            }*/
            printSharedData(sharedData);

        sem_post(sem_crt);
        sem_post(sem_rcstr);
        detach_memory_block(sharedData);
    }

    // Destroy the shared mem block and semaphores
    sem_close(sem_crt);
    sem_close(sem_clt);
    sem_close(sem_rcstr);
    destroy_memory_block(FILENAME);

    return 0;
}