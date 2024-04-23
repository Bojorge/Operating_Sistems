#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared_memory.h"

void printSharedData(SharedData *sharedData) {
    printf("Imprimiendo las estadisticas:\n");
    printf("sharedData->bufferSize: %d\n", sharedData->bufferSize);
    printf("sharedData->writeIndex: %d\n", sharedData->writeIndex);
    printf("sharedData->readIndex: %d\n", sharedData->readIndex);
    printf("sharedData->clientBlockedTime: %f ms\n", sharedData->clientBlockedTime);
    printf("sharedData->reconsBlockedTime: %f ms\n", sharedData->reconsBlockedTime);
    printf("sharedData->charsTransferred: %d\n", sharedData->charsTransferred);
    printf("sharedData->charsInBuffer: %d\n", sharedData->charsInBuffer);
    printf("sharedData->memUsed: %d\n", sharedData->memUsed);
    printf("sharedData->clientUserTime: %ld ms\n", sharedData->clientUserTime);
    printf("sharedData->clientKernelTime: %ld ms\n", sharedData->clientKernelTime);
    printf("sharedData->reconsUserTime: %ld ms\n", sharedData->recUserTime);
    printf("sharedData->reconsKernelTime: %ld ms\n", sharedData->recKernelTime);
}

int main(int argc, char *argv[]) 
{
    SharedData *sharedData = attach_struct(STRUCT_LOCATION, sizeof(SharedData));
    if (sharedData == NULL) {
        printf("Error al adjuntar al bloque de memoria compartida.\n");
        exit(EXIT_FAILURE);
    }

    sharedData->statsInited = true;
    printSharedData(sharedData);
}