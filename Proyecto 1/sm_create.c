#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>


#define MAX_CHARS 100
#define MEM_OBJ_NAME "/sharedMemory"

// Estructura para almacenar los datos compartidos
typedef struct {
    char buffer[MAX_CHARS]; // Buffer para almacenar caracteres
    int bufferSize; // Tamaño del buffer
} SharedMemory;

int main() {
    int numChars;
    printf("Ingrese la cantidad de caracteres a compartir: ");
    scanf("%d", &numChars);

    // Calcula el tamaño necesario para la memoria compartida
    size_t sharedSize = sizeof(SharedMemory);

    // Crea o abre el objeto de memoria compartida
    int fd = shm_open(MEM_OBJ_NAME, O_CREAT | O_RDWR, 0666);
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
    sharedMemory->bufferSize = numChars;
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

    return 0;
}
