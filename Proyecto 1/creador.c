#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared_memory.h"

void init_empty_struct (SharedData *sharedData, int numChars) {
    sharedData->bufferSize = numChars;
    sharedData->writeIndex = 0;
    sharedData->readIndex = 0;
    sharedData->readingFileIndex = 0;
    sharedData->clientBlockedTime = 0;
    sharedData->reconsBlockedTime = 0;
    sharedData->charsTransferred = 0;
    sharedData->charsInBuffer = 0;
    sharedData->clientUserTime = 0;
    sharedData->clientKernelTime = 0;
    sharedData->recUserTime = 0;
    sharedData->recKernelTime = 0;
    sharedData->memUsed = sizeof(SharedData) + (sizeof(Sentence) * numChars);
    sharedData->writingFinished = false;
    sharedData->statsInited = false;
    sharedData->clientEndedProcess = false;
    sharedData->reconsEndedProcess = false;
}

void init_struct_semaphores() {
    sem_unlink(SEM_W_INDEX);
    sem_unlink(SEM_R_INDEX);
    sem_unlink(SEM_RF_INDEX);
    sem_unlink(SEM_CB_TIME);
    sem_unlink(SEM_RB_TIME);
    sem_unlink(SEM_C_TRANSF);
    sem_unlink(SEM_C_BUFFER);

    sem_t *sem_writeIndex = sem_open(SEM_W_INDEX, O_CREAT, 0644, 1);
    sem_t *sem_readIndex = sem_open(SEM_R_INDEX, O_CREAT, 0644, 1);
    sem_t *sem_readFileIndex = sem_open(SEM_RF_INDEX, O_CREAT, 0644, 1);
    sem_t *sem_clientBlock = sem_open(SEM_CB_TIME, O_CREAT, 0644, 1);
    sem_t *sem_reconsBlock = sem_open(SEM_RB_TIME, O_CREAT, 0644, 1);
    sem_t *sem_characterTransf = sem_open(SEM_C_TRANSF, O_CREAT, 0644, 1);
    sem_t *sem_characterBuffer = sem_open(SEM_C_BUFFER, O_CREAT, 0644, 1);
}

void printSharedData(SharedData *sharedData) {
    printf("sharedData->bufferSize: %d\n", sharedData->bufferSize);
    printf("sharedData->writeIndex: %d\n", sharedData->writeIndex);
    printf("sharedData->readIndex: %d\n", sharedData->readIndex);
    printf("sharedData->clientBlockedTime: %d\n", sharedData->clientBlockedTime);
    printf("sharedData->recBlockedTime: %d\n", sharedData->reconsBlockedTime);
    printf("sharedData->charsTransferred: %d\n", sharedData->charsTransferred);
    printf("sharedData->charsInBuffer: %d\n", sharedData->charsInBuffer);
    printf("sharedData->memUsed: %d\n", sharedData->memUsed);
    printf("sharedData->clientUserTime: %d\n", sharedData->clientUserTime);
    printf("sharedData->clientKernelTime: %d\n", sharedData->clientKernelTime);
    printf("sharedData->recUserTime: %d\n", sharedData->recUserTime);
    printf("sharedData->recKernelTime: %d\n", sharedData->recKernelTime);
}

void printBuffer(Sentence *buffer, int numChars) {
    for (int i = 0; i < numChars; i++) {
            printf("buffer[%d] = \"%c\" | time: %s\n", i, buffer[i].character, buffer[i].time);
        }

        printf("--------------------------------------\n");
}

int main(int argc, char *argv[]) 
{
    destroy_memory_block(STRUCT_LOCATION);
    destroy_memory_block(BUFFER_LOCATION);
    
    int numChars;
    printf("Ingrese la cantidad de caracteres a compartir: ");
    scanf("%d", &numChars);

    // Set the semaphores
    for (int i = 0; i < numChars; i++) {
        // Make every semaphore name for each buffer space
        char sem_write_name[MAX_LENGTH];
        sprintf(sem_write_name, "%s%d", SEM_WRITE_VARIABLE_FNAME, i);

        char sem_read_name[MAX_LENGTH];
        sprintf(sem_read_name, "%s%d", SEM_READ_VARIABLE_FNAME, i);

        // Unlink them to prevent
        sem_unlink(sem_write_name);
        sem_unlink(sem_read_name);

        // Init each semaphore
        sem_t *sem_temp_write = sem_open(sem_write_name, O_CREAT, 0644, 1);
        if (sem_temp_write == SEM_FAILED) {
            perror("sem_open/variables");
            exit(EXIT_FAILURE);
        }

        sem_t *sem_temp_read = sem_open(sem_read_name, O_CREAT, 0644, 0);
        if (sem_temp_read == SEM_FAILED) {
            perror("sem_open/variables");
            exit(EXIT_FAILURE);
        }
    }

    // Initialize shared mem blocks
    init_mem_block(STRUCT_LOCATION, BUFFER_LOCATION, sizeof(SharedData), numChars * sizeof(Sentence));

    // Attach to struct shared mem block
    SharedData *sharedData = attach_struct(STRUCT_LOCATION, sizeof(SharedData));
    if (sharedData == NULL) {
        printf("Error al adjuntar al bloque de memoria compartida.\n");
        exit(EXIT_FAILURE);
    }

    // Init semaphores for struct variables
    init_struct_semaphores();

    // Attach to buffer mem block
    Sentence *buffer = attach_buffer(BUFFER_LOCATION, numChars * sizeof(Sentence));
    if (buffer == NULL) {
        printf("Error al adjuntar al bloque de memoria compartida.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize empty struct for the shared mem block
    init_empty_struct(sharedData, numChars);
    

    // Start visualization of mem block
    while(true) {
        printSharedData(sharedData);
        printBuffer(buffer, numChars);
        sleep(1);
    }

    detach_struct(sharedData);
    detach_buffer(buffer);

    // Destroy the shared mem block
    destroy_memory_block(STRUCT_LOCATION);
    destroy_memory_block(BUFFER_LOCATION);

    return 0;
}
