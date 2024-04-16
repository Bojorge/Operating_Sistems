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
void initializeCircularBuffer(CircularBuffer *cb, size_t size) {
    cb->buffer = (char *)malloc(size * sizeof(char));
    if (cb->buffer == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el buffer.\n");
        exit(EXIT_FAILURE);
    }
    cb->bufferSize = size;
    cb->writeIndex = 0;
    cb->readIndex = 0;
    cb->memUsed = 0;
}

// Función para destruir el buffer circular
void destroyCircularBuffer(CircularBuffer *cb) {
    free(cb->buffer);
}

// Función para escribir un carácter en el buffer
int writeChar(CircularBuffer *cb, char c) {
    if (cb->memUsed < cb->bufferSize) {
        cb->buffer[cb->writeIndex] = c;
        cb->writeIndex = (cb->writeIndex + 1) % cb->bufferSize;
        cb->memUsed++;
        return 1;
    } else {
        printf("\n \n  ***   El buffer se llenó   ***\n");
        return 0;
    }
}

// Función para mostrar el contenido del buffer
void printBuffer(CircularBuffer *cb) {
    printf("Contenido del buffer: ");
    if (cb->memUsed == 0) {
        printf("El buffer está vacío.\n");
        return;
    }
    int i = cb->readIndex;
    int count = 0;
    while (count < cb->memUsed) {
        printf("%c ", cb->buffer[i]);
        i = (i + 1) % cb->bufferSize;
        count++;
    }
    printf("\n");
}

int main() {
    // Tamaño del buffer circular
    size_t bufferSize = 10;
    
    // Inicializar el buffer circular
    CircularBuffer cb;
    initializeCircularBuffer(&cb, bufferSize);
    
    // Bucle principal para ingresar caracteres
    char input;
    printf("Ingrese caracteres (ingrese '.' para salir):\n");
    while (1) {
        scanf(" %c", &input);
        if (input == '.') {
            break;
        }
        if (!writeChar(&cb, input)) {
            break; // Salir si el buffer está lleno
        }
        printBuffer(&cb);
    }
    
    // Destruir el buffer circular
    destroyCircularBuffer(&cb);
    
    return 0;
}


