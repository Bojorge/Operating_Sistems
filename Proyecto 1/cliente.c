#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared_memory.h"

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

    // Read the file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error opening file %s\n",argv[1]);
        return -1;
    }

    printf("sharedData->bufferSize: %d\n", sharedData->bufferSize);

    char* temp_buffer[sharedData->bufferSize];
    char* temp_times[sharedData->bufferSize * 24];

    /*memcpy(temp_buffer, sharedData->buffer, (sharedData->bufferSize * sizeof(char)));
    memcpy(temp_times, sharedData->insertTimes, (sharedData->bufferSize * sizeof(time_t)));
    printf("temp_buffer[0]: %s", temp_buffer[0]);
    printf("temp_times[0]: %ln", temp_times[0]);*/

    char character;
    for (; (character = fgetc(file)) != EOF;) {
        sem_wait(sem_crt);

        printf("Agregando caracter: %c\n", character);

        time_t current_time;
        time(&current_time);

        temp_buffer[sharedData->writeIndex] = &character;
        temp_times[sharedData->writeIndex] = ctime(&current_time);

        sharedData->charsTransferred++;
        sharedData->charsRemaining++;
        sharedData->writeIndex++;

        sem_post(sem_clt);
        sem_post(sem_rcstr);
    }

    for (int i = 0; i < sharedData->bufferSize; ++i) {
        printf("buffer: %s\n", temp_buffer[i]);
        printf("times: %s\n", temp_times[i]);
    }

    sharedData->buffer = temp_buffer;
    sharedData->insertTimes = temp_times;

    // Destroy semaphores and detach from memory after finishing
    sem_close(sem_crt);
    sem_close(sem_clt);
    sem_close(sem_rcstr);
    detach_memory_block(sharedData);

    return 0;
}