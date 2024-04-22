#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>

// Variables
#define STRUCT_LOCATION "creador.c"
#define BUFFER_LOCATION "destroy.c"
#define IPC_RESULT_ERROR (-1)
#define MAX_TIME_LENGTH 21

// Structs
typedef struct {
    char character;
    char time[MAX_TIME_LENGTH];
} Sentence;

// Functions
void init_mem_block(char *struct_location, char *buffer_location, int sizeStruct, int sizeBuffer);
Sentence * attach_buffer(char *buffer_location, int size);
bool detach_buffer(Sentence *buffer);
bool destroy_memory_block(char *location);
void write_to_buffer(int numChars);

int main(int argc, char *argv[]) 
{
    destroy_memory_block(STRUCT_LOCATION);
    destroy_memory_block(BUFFER_LOCATION);

    int numChars;
    printf("Ingrese la cantidad de caracteres a compartir: ");
    scanf("%d", &numChars);

    // Initialize shared mem blocks
    init_mem_block(STRUCT_LOCATION, BUFFER_LOCATION, sizeof(Sentence), numChars * sizeof(Sentence));

    // Escribir en el buffer
    write_to_buffer(numChars);

    // Adjuntar el buffer
    Sentence *buffer = attach_buffer(BUFFER_LOCATION, numChars * sizeof(Sentence));

    if (buffer == NULL) {
        printf("Error al adjuntar al bloque de memoria compartida.\n");
        exit(EXIT_FAILURE);
    }

    // Imprimir valores del buffer
    for (int i = 0; i < numChars; i++) {
        printf("buffer[%d] = %c | time: %s\n", i, buffer[i].character, buffer[i].time);
    }

    return 0;
}

static int get_shared_block(char *location, int size) {
    key_t key;
    key = ftok(location, 0);

    if (key == IPC_RESULT_ERROR) {
        return IPC_RESULT_ERROR;
    }

    return shmget(key, size, 0644 | IPC_CREAT);
}

void init_mem_block(char *struct_location, char *buffer_location, int sizeStruct, int sizeBuffer) {
    int struct_block_id = get_shared_block(struct_location, sizeStruct);
    int buffer_block_id = get_shared_block(buffer_location, sizeBuffer);

    if (struct_block_id == IPC_RESULT_ERROR) {
        printf("Error al obtener identificador del bloque compartido struct.\n");
        exit(EXIT_FAILURE);
    }

    if (buffer_block_id == IPC_RESULT_ERROR) {
        printf("Error al obtener identificador del bloque compartido buffer.\n");
        exit(EXIT_FAILURE);
    }
}

Sentence * attach_buffer(char *buffer_location, int size) {
    int shared_block_id = get_shared_block(buffer_location, size);

    if (shared_block_id == IPC_RESULT_ERROR) {
        return NULL;
    }

    Sentence *sharedData = shmat(shared_block_id, NULL, 0);

    return sharedData;
}

bool detach_buffer(Sentence *buffer) {
    return (shmdt(buffer) != -1);
}

bool destroy_memory_block(char *location) {
    int shared_block_id = get_shared_block(location, 0);

    if (shared_block_id == IPC_RESULT_ERROR) {
        return NULL;
    }

    return (shmctl(shared_block_id, IPC_RMID, NULL) != IPC_RESULT_ERROR);
}

void write_to_buffer(int numChars) {
    // Attach to buffer mem block
    Sentence *buffer = attach_buffer(BUFFER_LOCATION, numChars * sizeof(Sentence));

    if (buffer == NULL) {
        printf("Error al adjuntar al bloque de memoria compartida.\n");
        exit(EXIT_FAILURE);
    }

    // Establecer el tiempo fijo
    char fixed_time[MAX_TIME_LENGTH] = "Apr 19 2024 19:00:00";

    for (int i = 0; i < numChars; i++) {
        buffer[i].character = 'a' + i;
        strcpy(buffer[i].time, fixed_time);
    }

    // Detach del buffer
    detach_buffer(buffer);
}
