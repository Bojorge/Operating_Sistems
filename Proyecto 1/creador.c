#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define MEMORY_OBJECT_NAME "/sharedMemory"

typedef struct {
    char buffer[100]; // Buffer para almacenar caracteres
    int bufferSize;         // Tamaño del buffer
} SharedMemory;

void initializeSharedMemory(SharedMemory *sm, int bufferSize) {
    sm->bufferSize = bufferSize;
    memset(sm->buffer, '\0', bufferSize); // Inicializa el buffer con caracteres nulos
}

int main() {
    int bufferSize;
    printf("Ingrese la cantidad de caracteres a compartir: ");
    scanf("%d", &bufferSize);

    size_t sharedSize = sizeof(char) * bufferSize;

    printf("Tamaño de la memoria compartida: %zu bytes\n", sharedSize);

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

    initializeSharedMemory(sharedMemory, bufferSize);

    printf("Visualización en tiempo real del contenido de la memoria compartida:\n");
    while (1) {
        printf("Buffer: %s\n", sharedMemory->buffer);
        sleep(1); // Espera 1 segundo antes de volver a visualizar
    }

    munmap(sharedMemory, sharedSize);
    close(fd);

    return 0;
}
