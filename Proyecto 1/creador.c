// creador.c


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>


#include "shared_memory.c"

int main() {
    int numChars;
    printf("Ingrese la cantidad de caracteres a compartir: ");
    scanf("%d", &numChars);

    // Calcula el tamaño necesario para la memoria compartida
    size_t sharedSize = sizeof(char)*numChars;
    printf("Tamaño de la memoria compartida: %zu bytes\n", sharedSize);

    initializeCircularBuffer(numChars, sharedSize);

    return 0;
}


