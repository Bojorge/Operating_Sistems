#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estructura para el buffer circular
typedef struct {
    char *buffer;
    size_t bufferSize;
    int writeIndex; // Head
    int readIndex;  // Tail
    int memUsed;    // Count
} CircularBuffer;

// Función para inicializar el buffer circular
void initializeCircularBuffer(CircularBuffer *cb, size_t size);

// Función para destruir el buffer circular
void destroyCircularBuffer(CircularBuffer *cb);

// Función para escribir un carácter en el buffer
int writeChar(CircularBuffer *cb, char c);

// Función para mostrar el contenido del buffer
void printBuffer(CircularBuffer *cb);

#endif /* CIRCULAR_BUFFER_H */
