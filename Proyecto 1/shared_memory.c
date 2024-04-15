// shared_memory.c


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>

#include "shared_memory.h"


// Función para inicializar el buffer circular
void initializeCircularBuffer(int numChars, size_t sharedSize) {
    // Crea o abre el objeto de memoria compartida
    int fd = shm_open(MEMORY_OBJECT_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("Error al crear/abrir el objeto de memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Establece el tamaño del objeto de memoria compartida
    if (ftruncate(fd, sharedSize) == -1) {
        perror("Error al establecer el tamaño de la memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Mapea la memoria compartida a la estructura SharedMemory
    SharedMemory *sharedMemory = (SharedMemory *)mmap(NULL, sharedSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sharedMemory == MAP_FAILED) {
        perror("Error al mapear la memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Inicializa la estructura SharedMemory
    sharedMemory->bufferSize = sharedSize;
    memset(sharedMemory->buffer, '\0', MAX_CHARS); // Llena el buffer con caracteres nulos

    // Visualiza el contenido de la memoria compartida
    printf("Visualización en tiempo real del contenido de la memoria compartida:\n");
    while (1) {
        printf("Buffer: %s\n", sharedMemory->buffer);
        sleep(1); // Espera 1 segundo antes de volver a visualizar
    }

    // Desvincula y cierra la memoria compartida
    munmap(sharedMemory, sharedSize);
    close(fd);
}




void write_buf(char buf[]){
    printf("%d \n", (int)sizeof(buf));
    char *ptr;
   
    int fd = shm_open (MEMORY_OBJECT_NAME,  O_RDWR  , 00200); /* open s.m object*/
    if(fd == -1)
    {
        printf("Error file descriptor %s\n", strerror(errno));
        exit(1);
    }
    
    ptr = mmap(NULL, sizeof(buf), PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED)
    {
        printf("Map failed in write process: %s\n", strerror(errno));
        exit(1);
    }
    
    memcpy(ptr,buf, sizeof(buf));
    //printf("%d \n", (int)sizeof(buf));
    close(fd);
   
}

