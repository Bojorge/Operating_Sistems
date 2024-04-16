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
    // Tamaño del buffer circular
    size_t bufferSize = 10;
    
    // Inicializar el buffer circular
    SharedMemory *sm = malloc(sizeof(SharedMemory)); // Cambiado de SharedMemory sm; a SharedMemory *sm;
    if (sm == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria para la estructura SharedMemory.\n");
        return EXIT_FAILURE;
    }
    initializeCircularBuffer(sm, bufferSize);
    
    
    // Bucle principal para ingresar caracteres
    char input;
    printf("Ingrese caracteres (ingrese '.' para salir):\n");
    while (1) {
        scanf("\n %c", &input);
        if (input == '.') {
            break;
        }
        if (!writeChar(sm, input)) {
            break; // Salir si el buffer está lleno
        }
        printBuffer(sm);
    }
    
    // Destruir el buffer circular
    destroyCircularBuffer(sm);
    free(sm); // Liberar memoria asignada a la estructura SharedMemory
    
    return 0;
}
