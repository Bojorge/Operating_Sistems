#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
 
#include "shared_memory.c"

int main(void)
{
    int bufferSize = getBufferSize();

    // Leer el archivo de texto especificado por el usuario
    char filename[20];
    printf("Ingrese el nombre del archivo de texto: ");
    scanf("%s", filename);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    // Leer el contenido inicial del archivo
    char buf[bufferSize];
    fgets(buf, bufferSize, file);
    printf("Contenido inicial del archivo: %s\n", buf);



    //size_t size = sizeof(buf); // Obtener el tamaño del array buf en bytes
    size_t size = bufferSize; 
    printf("Tamaño del array buf: %zu bytes\n", size);

    write_buf(buf, size);
   
   return 0;
}
