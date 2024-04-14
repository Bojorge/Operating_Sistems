#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define MAX_CHARS 100
#define MEM_OBJ_NAME "/sharedMemory"

typedef struct {
    char buffer[MAX_CHARS];
    int bufferSize;
} SharedMemory;

// Función para mostrar la reconstrucción del archivo
void showReconstruction(char character, int position, time_t timestamp) {
    printf("Carácter '%c' insertado en la posición %d a las %s", character, position, ctime(&timestamp));
}

int main() {
    int fd;
    SharedMemory *sharedMemory;
    
    // Abrir el objeto de memoria compartida en modo de solo lectura
    fd = shm_open(MEM_OBJ_NAME, O_RDONLY, 0666);
    if (fd == -1) {
        perror("Error al abrir el objeto de memoria compartida");
        exit(EXIT_FAILURE);
    }
    
    // Mapear la memoria compartida
    sharedMemory = (SharedMemory *)mmap(NULL, sizeof(SharedMemory), PROT_READ, MAP_SHARED, fd, 0);
    if (sharedMemory == MAP_FAILED) {
        perror("Error al mapear la memoria compartida");
        exit(EXIT_FAILURE);
    }
    
    // Mostrar el proceso de reconstrucción del archivo
    printf("Proceso de reconstrucción del archivo:\n");
    while (1) {
        // Leer los caracteres de la memoria compartida
        for (int i = 0; i < sharedMemory->bufferSize; i++) {
            // Mostrar la reconstrucción caracter por caracter
            showReconstruction(sharedMemory->buffer[i], i, time(NULL));
        }
        // Esperar un tiempo antes de mostrar el siguiente paso de reconstrucción
        sleep(1);
    }
    
    // Desvincular la memoria compartida y cerrar el descriptor de archivo
    munmap(sharedMemory, sizeof(SharedMemory));
    close(fd);
    
    return 0;
}
