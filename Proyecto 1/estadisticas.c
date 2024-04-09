#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define STATS_MEM_OBJ_NAME "/statsMemory"

typedef struct {
    double clientBlockedTime;      // Tiempo bloqueado del cliente
    double reconstructorBlockedTime; // Tiempo bloqueado del Re-constructor
    int transferredChars;          // Caracteres transferidos
    int remainingChars;            // Caracteres que quedan en el buffer
    size_t totalMemoryUsed;        // Espacio total de memoria utilizado
    double clientUserTime;         // Tiempo total en modo usuario del cliente
    double clientKernelTime;       // Tiempo total en modo kernel del cliente
    double reconstructorUserTime;  // Tiempo total en modo usuario del Re-constructor
    double reconstructorKernelTime;// Tiempo total en modo kernel del Re-constructor
} Statistics;

int main() {
    int fd;
    Statistics *stats;
    
    // Crear o abrir el objeto de memoria compartida para las estadísticas
    fd = shm_open(STATS_MEM_OBJ_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("Error al crear/abrir el objeto de memoria compartida");
        exit(EXIT_FAILURE);
    }
    
    // Establecer el tamaño del objeto de memoria compartida
    if (ftruncate(fd, sizeof(Statistics)) == -1) {
        perror("Error al establecer el tamaño de la memoria compartida");
        exit(EXIT_FAILURE);
    }
    
    // Mapear la memoria compartida
    stats = (Statistics *)mmap(NULL, sizeof(Statistics), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (stats == MAP_FAILED) {
        perror("Error al mapear la memoria compartida");
        exit(EXIT_FAILURE);
    }
    
    // Inicializar las estadísticas
    stats->clientBlockedTime = 0.0;
    stats->reconstructorBlockedTime = 0.0;
    stats->transferredChars = 0;
    stats->remainingChars = 0;
    stats->totalMemoryUsed = 0;
    stats->clientUserTime = 0.0;
    stats->clientKernelTime = 0.0;
    stats->reconstructorUserTime = 0.0;
    stats->reconstructorKernelTime = 0.0;
    
    // Mostrar las estadísticas
    printf("Estadísticas:\n");
    printf("Tiempo bloqueado del cliente: %.2f segundos\n", stats->clientBlockedTime);
    printf("Tiempo bloqueado del Re-constructor: %.2f segundos\n", stats->reconstructorBlockedTime);
    printf("Caracteres transferidos: %d\n", stats->transferredChars);
    printf("Caracteres que quedan en el buffer: %d\n", stats->remainingChars);
    printf("Espacio total de memoria utilizado: %zu bytes\n", stats->totalMemoryUsed);
    printf("Tiempo total en modo usuario del cliente: %.2f segundos\n", stats->clientUserTime);
    printf("Tiempo total en modo kernel del cliente: %.2f segundos\n", stats->clientKernelTime);
    printf("Tiempo total en modo usuario del Re-constructor: %.2f segundos\n", stats->reconstructorUserTime);
    printf("Tiempo total en modo kernel del Re-constructor: %.2f segundos\n", stats->reconstructorKernelTime);
    
    // Desvincular la memoria compartida y cerrar el descriptor de archivo
    munmap(stats, sizeof(Statistics));
    close(fd);
    
    return 0;
}
