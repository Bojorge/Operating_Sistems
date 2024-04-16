#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <linux/time.h>

#define MEMORY_OBJECT_NAME "/sharedMemory"

int main(void) {
    int fd;
    char input;
    struct timespec current_time;
    
    fd = shm_open(MEMORY_OBJECT_NAME, O_RDWR, 0666);
    if (fd == -1) {
        perror("Error al abrir el objeto de memoria compartida");
        exit(EXIT_FAILURE);
    }
    
    char *ptr = mmap(NULL, sizeof(char), PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Error al mapear la memoria compartida");
        exit(EXIT_FAILURE);
    }
    
    printf("Ingrese caracteres (ingrese '.' para salir):\n");
    while (1) {
        scanf("\n%c", &input);
        if (input == '.') {
            break;
        }
        clock_gettime(CLOCK_REALTIME, &current_time);
        printf("Carácter insertado: %c, Hora: %ld:%ld\n", input, current_time.tv_sec, current_time.tv_nsec);
        *ptr = input;
        ptr++;
    }

    munmap(ptr, sizeof(char));
    close(fd);
   
    return 0;
}
