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

// Función para insertar un carácter en la memoria compartida de manera circular
void insertCharacterCircular(char character, SharedMemory *sharedMemory) {
    // Obtener el índice donde se insertará el próximo carácter
    int index = sharedMemory->bufferSize % MAX_CHARS;

    // Insertar el carácter en el buffer
    sharedMemory->buffer[index] = character;

    // Incrementar el tamaño del buffer
    sharedMemory->bufferSize++;

    // Mostrar el carácter, la hora y la posición donde se insertó
    time_t currentTime;
    time(&currentTime);
    printf("Caracter '%c' insertado en la posición %d a las %s", character, index, ctime(&currentTime));
}

int main() {
    // Leer el archivo de texto especificado por el usuario
    char filename[100];
    printf("Ingrese el nombre del archivo de texto: ");
    scanf("%s", filename);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    // Leer el contenido inicial del archivo
    char initialContent[MAX_CHARS];
    fgets(initialContent, MAX_CHARS, file);
    printf("Contenido inicial del archivo: %s\n", initialContent);

    // Acceder a la memoria compartida creada por el proceso creador
    int fd = shm_open(MEM_OBJ_NAME, O_RDWR, 0666);
    if (fd == -1) {
        perror("Error al abrir el objeto de memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Mapear la memoria compartida
    SharedMemory *sharedMemory = (SharedMemory *)mmap(NULL, sizeof(SharedMemory), PROT_WRITE, MAP_SHARED, fd, 0);
    if (sharedMemory == MAP_FAILED) {
        perror("Error al mapear la memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Almacenar los caracteres del archivo de texto en la memoria compartida de manera circular
    char character;
    while ((character = fgetc(file)) != EOF) {
        // Insertar el carácter en la memoria compartida de manera circular
        insertCharacterCircular(character, sharedMemory);
        // Mostrar los caracteres que se están introduciendo en la memoria compartida
        printf("Contenido en la memoria compartida: %s\n", sharedMemory->buffer);
    }

    // Proporcionar un mecanismo para terminar el proceso de manera elegante
    fclose(file);
    munmap(sharedMemory, sizeof(SharedMemory));
    close(fd);

    return 0;
}
