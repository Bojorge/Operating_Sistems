// shared_memory.h


#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#define MEMORY_OBJECT_NAME "/sharedProcessMemory"

typedef struct {
    char *buffer;
    size_t bufferSize;
    int writeIndex; // Head
    int readIndex;  // Tail
    int memUsed;    // Count
} SharedMemory;

// Función para inicializar el buffer circular
void initializeCircularBuffer(SharedMemory *sm, size_t size);

// Función para destruir el buffer circular
void destroyCircularBuffer(SharedMemory *sm);

// Función para escribir un carácter en el buffer
int writeChar(SharedMemory *sm, char c);

// Función para mostrar el contenido del buffer
void printBuffer(SharedMemory *sm);

#endif


