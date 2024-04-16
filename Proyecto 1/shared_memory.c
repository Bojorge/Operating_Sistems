// shared_memory.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

#include "shared_memory.h"

void initializeCircularBuffer(int numChars, size_t sharedSize) {
    int fd = shm_open(MEMORY_OBJECT_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("Error al crear/abrir el objeto de memoria compartida");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, sharedSize) == -1) {
        perror("Error al establecer el tamaño de la memoria compartida");
        exit(EXIT_FAILURE);
    }

    SharedMemory *sharedMemory = (SharedMemory *)mmap(NULL, sharedSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sharedMemory == MAP_FAILED) {
        perror("Error al mapear la memoria compartida");
        exit(EXIT_FAILURE);
    }

    sharedMemory->bufferSize = numChars;
    sharedMemory->writeIndex = 0;
    sharedMemory->readIndex = 0;
    sharedMemory->memUsed = 0;

    // Asigna memoria para el buffer
    sharedMemory->buffer = (char *)malloc(numChars * sizeof(char));
    if (sharedMemory->buffer == NULL) {
        perror("Error al asignar memoria para el buffer");
        exit(EXIT_FAILURE);
    }
    memset(sharedMemory->buffer, '\0', numChars); // Llena el buffer con caracteres nulos

    // Visualiza el contenido de la memoria compartida
    printf("Visualización en tiempo real del contenido de la memoria compartida:\n");
    while (1) {
        printf("\r Contenido del buffer: %s", sharedMemory->buffer);
        fflush(stdout);
        sleep(1); // Espera 1 segundo antes de volver a visualizar
    }
    
    munmap(sharedMemory, sharedSize);
    close(fd);
}

void write_buf(char buf[], size_t size) {
    int fd = shm_open(MEMORY_OBJECT_NAME, O_RDWR, 0666);
    if (fd == -1) {
        perror("Error al abrir el objeto de memoria compartida");
        exit(EXIT_FAILURE);
    }

    SharedMemory *sharedMemory = (SharedMemory *)mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sharedMemory == MAP_FAILED) {
        perror("Error al mapear la memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Inicializar buffer si no está inicializado
    if (sharedMemory->buffer == NULL) {
        sharedMemory->buffer = (char *)malloc(sharedMemory->bufferSize * sizeof(char));
        if (sharedMemory->buffer == NULL) {
            perror("Error al asignar memoria para el buffer");
            munmap(sharedMemory, sizeof(SharedMemory));
            close(fd);
            exit(EXIT_FAILURE);
        }
        memset(sharedMemory->buffer, '\0', sharedMemory->bufferSize);
    }

    if (size > sharedMemory->bufferSize) {
        printf("Error: Tamaño de escritura excede el tamaño del buffer.\n");
        munmap(sharedMemory, sizeof(SharedMemory));
        close(fd);
        return;
    }

    int writeIndex = sharedMemory->writeIndex;
    memcpy(sharedMemory->buffer + writeIndex, buf, size);

    sharedMemory->writeIndex = (writeIndex + size) % sharedMemory->bufferSize;
    sharedMemory->memUsed += size;

    munmap(sharedMemory, sizeof(SharedMemory));
    close(fd);
}

size_t getBufferSize() {
    int fd = shm_open(MEMORY_OBJECT_NAME, O_RDONLY, 0666);
    if (fd == -1) {
        perror("Error al abrir el objeto de memoria compartida");
        exit(EXIT_FAILURE);
    }

    SharedMemory *sharedMemory = (SharedMemory *)mmap(NULL, sizeof(SharedMemory), PROT_READ, MAP_SHARED, fd, 0);
    if (sharedMemory == MAP_FAILED) {
        perror("Error al mapear la memoria compartida");
        exit(EXIT_FAILURE);
    }

    size_t bufferSize = sharedMemory->bufferSize;

    munmap(sharedMemory, sizeof(SharedMemory));
    close(fd);

    return bufferSize;
}

