#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared_memory.h"

int main(int argc, char *argv[])
{
    // Check for specified file and mode same as interval
    if (argc < 2 || argc > 3) {
        printf("Uso: %s <modo> [intervalo]s\n", argv[0]);
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
    char *block = attach_memory_block(FILENAME, BLOCK_SIZE);
    if (block == NULL) {
        printf("ERROR: no se pudo acceder al bloque\n");
        return -1;
    }

    SharedData *sharedData;

    // Create For Loop Here That Reads Buffer And Add
    // Character By Character To A File To Reconstruct
    // Initial File
    sem_wait(sem_clt);
    printf("Reading: \"%c\"\n", sharedData->buffer[0]);
    sem_post(sem_clt);
    sem_post(sem_rcstr);
    
    // Destroy semaphores and detach from memory after finishing
    sem_close(sem_crt);
    sem_close(sem_clt);
    sem_close(sem_rcstr);
    detach_memory_block(block);

    return 0;
}