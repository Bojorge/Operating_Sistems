#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared_memory.h"

void printSharedData(SharedData *sharedData) {
    printf("sharedData->buffer: %d\n", sharedData->buffer);
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
    printf("sharedData->recKernelTime: %d\n", sharedData->recKernelTime);

    /*printf("Contenido del buffer: \n");
        for (int i = 0; i < sharedData->bufferSize; i++) {
            printf("\"%c\" en posicion: %d ", sharedData->buffer[i], i);
            printf("insertado a las: %ld\n", sharedData->insertTimes[i]);
        }*/
}

int main(int argc, char *argv[]) 
{   
    // Check for specified file and mode same as interval
    if (argc < 3 || argc > 4) {
        printf("Uso: %s <archivo> <modo> [intervalo]s\n", argv[0]);
        printf("Modo: 0 = Manual, 1 = Automático\n");
        return -1;
    }

    // Open semaphores that were already created
    sem_t *sem_crt = sem_open(SEM_CREATOR_FNAME, 0);
    if (sem_crt == SEM_FAILED) {
        perror("sem_open/creator");
        exit(EXIT_FAILURE);
    }
    sem_t *sem_clt = sem_open(SEM_CLIENT_FNAME, 0);
    if (sem_clt == SEM_FAILED) {
        perror("sem_open/client");
        exit(EXIT_FAILURE);
    }
    sem_t *sem_rcstr = sem_open(SEM_RECONSTRUCTOR_FNAME, 0);
    if (sem_rcstr == SEM_FAILED) {
        perror("sem_open/reconstructor");
        exit(EXIT_FAILURE);
    }

    // Connect to shared mem block
    SharedData *sharedData = attach_memory_block(FILENAME, BLOCK_SIZE);
    if (sharedData == NULL) {
        printf("ERROR: no se pudo acceder al bloque\n");
        return -1;
    }

    printSharedData(sharedData);

    // Read the file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error opening file %s\n",argv[1]);
        return -1;
    }

    int character;
    for (; (character = fgetc(file)) != EOF;) {
        sem_wait(sem_crt);

        printf("Agregando caracter: %c\n", character);

        //strncpy(sharedData->buffer[sharedData->writeIndex], character, sizeof(char));

        //sharedData->buffer[sharedData->writeIndex] = character;
        /*sharedData->insertTimes[sharedData->writeIndex] = time(NULL);
        sharedData->charsTransferred++;
        sharedData->charsRemaining++;
        sharedData->writeIndex++;*/

        sem_post(sem_clt);
        sem_post(sem_rcstr);
    }

    // Create For Loop Here That Reads File And Add
    // Character By Character To The Circular Buffer

    // Destroy semaphores and detach from memory after finishing
    sem_close(sem_crt);
    sem_close(sem_clt);
    sem_close(sem_rcstr);
    detach_memory_block(sharedData);

    return 0;
}