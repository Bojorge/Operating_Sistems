// shared_memory.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

#include "shared_memory.h"

// Función para inicializar el buffer circular
void initializeCircularBuffer(SharedMemory *sm, size_t size) {
    sm->buffer = (char *)malloc(size * sizeof(char));
    if (sm->buffer == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el buffer.\n");
        exit(EXIT_FAILURE);
    }
    sm->bufferSize = size;
    sm->writeIndex = 0;
    sm->readIndex = 0;
    sm->memUsed = 0;
}

// Función para destruir el buffer circular
void destroyCircularBuffer(SharedMemory *sm) {
    free(sm->buffer);
}

// Función para escribir un carácter en el buffer
int writeChar(SharedMemory *sm, char c) {
    if (sm->memUsed < sm->bufferSize) {
        sm->buffer[sm->writeIndex] = c;
        sm->writeIndex = (sm->writeIndex + 1) % sm->bufferSize;
        sm->memUsed++;
        return 1;
    } else {
        printf("\n \n  ***   El buffer se llenó   ***\n");
        return 0;
    }
}

// Función para mostrar el contenido del buffer
void printBuffer(SharedMemory *sm) {
    printf("Contenido del buffer: ");
    if (sm->memUsed == 0) {
        printf("El buffer está vacío.\n");
        return;
    }
    int i = sm->readIndex;
    int count = 0;
    while (count < sm->memUsed) {
        printf("%c ", sm->buffer[i]);
        //fflush(stdout);
        i = (i + 1) % sm->bufferSize;
        count++;
    }
    printf("\n");
}


