#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include "shared_memory.h" // Cambiado de shared_memory.c a shared_memory.h

int main() {
    int numChars;
    printf("Ingrese la cantidad de caracteres a compartir: ");
    scanf("%d", &numChars);

    // Calcula el tamaño necesario para la memoria compartida
    size_t bufferSize = sizeof(char)*numChars;

    printf("Tamaño de la memoria compartida: %zu bytes\n", bufferSize);

    // Crea o abre el objeto de memoria compartida
    int fd = shm_open(MEMORY_OBJECT_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("Error al crear/abrir el objeto de memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Establece el tamaño del objeto de memoria compartida
    if (ftruncate(fd, bufferSize) == -1) {
        perror("Error al establecer el tamaño de la memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Mapea la memoria compartida a la estructura SharedMemory
    SharedMemory *sharedMemory = (SharedMemory *)mmap(NULL, bufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sharedMemory == MAP_FAILED) {
        perror("Error al mapear la memoria compartida");
        exit(EXIT_FAILURE);
    }






    initializeCircularBuffer(sharedMemory, bufferSize);
    

    // Bucle principal para ingresar caracteres
    char input;
    printf("Ingrese caracteres (ingrese '.' para salir):\n");
    while (1) {
        scanf("\n %c", &input);
        if (input == '.') {
            break;
        }
        if (!writeChar(sharedMemory, input)) {
            break; // Salir si el buffer está lleno
        }
        printBuffer(sharedMemory);
    }
    
    // Destruir el buffer circular
    destroyCircularBuffer(sharedMemory);
    
    return 0;
}
