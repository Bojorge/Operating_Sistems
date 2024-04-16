#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared_memory.h"

void read_manually(sem_t *sem_crt, sem_t *sem_clt, sem_t *sem_rcstr, SharedData *sharedData, Sentence *buffer) {
   
   // Variables para manejo de string reconstruido
   char *dynamic_string = NULL;
   size_t buffer_size = 0;
   size_t length = 0;

    sem_wait(sem_crt);
    
    // Get semaphore for said reading space, to check if reading is available
    char sem_read_name[MAX_LENGTH];
    sprintf(sem_read_name, "%s%d", SEM_READ_VARIABLE_FNAME, sharedData->writeIndex);

    sem_t *sem_var_read = sem_open(sem_read_name, 0);
    if (sem_var_read == SEM_FAILED) {
        perror("sem_open/variables");
        exit(EXIT_FAILURE);
    }

    // Get semaphore for said writing space, to post after reading
    char sem_write_name[MAX_LENGTH];
    sprintf(sem_write_name, "%s%d", SEM_WRITE_VARIABLE_FNAME, sharedData->writeIndex);

    sem_t *sem_var_write = sem_open(sem_write_name, 0);
    if (sem_var_write == SEM_FAILED) {
        perror("sem_open/variables");
        exit(EXIT_FAILURE);
    }

    // Write after checking if semaphore is open
    sem_wait(sem_var_read);

    // Obtener indice de lectura, leer e imprimir dato encontrado
    int index = sharedData->readIndex;
    printf("Reading from buffer: \nbuffer[%d] = \"%c\" | time: %s\n", index, buffer[index].character, buffer[index].time);
    
    // Manejo string reconstruido
    buffer_size++;
    char *temp = realloc(dynamic_string, buffer_size * sizeof(char));
    if (temp == NULL) {
        // La realocación falló, manejar el error según sea necesario
        fprintf(stderr, "Error: No se pudo realocar la memoria\n");
        free(dynamic_string); // Liberamos la memoria previamente asignada
        exit(EXIT_FAILURE);
    }
    dynamic_string = temp;

    dynamic_string[length++] = buffer[index].character;
    dynamic_string[length] = '\0';

    printf("%s", dynamic_string);


    // Actualizar los indices compartidos
    sharedData->charsTransferred++;
    sharedData->readIndex = (sharedData->readIndex + 1) % sharedData->bufferSize;

    // Post so that variable could be written in
    sem_post(sem_var_write);

    // Post all semaphores so that other processes take control of shared mem
    sem_post(sem_rcstr);
    sem_post(sem_clt);
    sharedData->writingFinished = true;
    free(dynamic_string);
}

int main(int argc, char *argv[])
{
    // Check for specified file and mode same as interval
    if (argc < 2 || argc > 3) {
        printf("Uso: %s <modo> [intervalo]s\n", argv[0]);
        printf("Modo: 0 = Manual, 1 = Automático\n");
        return -1;
    }

    // Open semaphores that were already created
    sem_t *sem_crt = sem_open(SEM_CREATOR_FNAME, 0);
    if (sem_crt == SEM_FAILED) {
        perror("sem_open/creator");
        exit(EXIT_FAILURE);
    }
    sem_t *sem_clt = sem_open(SEM_CLIENT_FNAME, 0);
    if (sem_clt == SEM_FAILED) {
        perror("sem_open/client");
        exit(EXIT_FAILURE);
    }
    sem_t *sem_rcstr = sem_open(SEM_RECONSTRUCTOR_FNAME, 0);
    if (sem_rcstr == SEM_FAILED) {
        perror("sem_open/reconstructor");
        exit(EXIT_FAILURE);
    }

    // Connect to shared mem block
    SharedData *sharedData = attach_struct(STRUCT_LOCATION, sizeof(SharedData));
    if (sharedData == NULL) {
        printf("ERROR: no se pudo acceder al bloque\n");
        return -1;
    }

    Sentence *buffer = attach_buffer(BUFFER_LOCATION, (sharedData->bufferSize * sizeof(Sentence)));
    if (buffer == NULL) {
        printf("ERROR: no se pudo acceder al bloque\n");
        return -1;
    }

    // Create For Loop Here That Reads Buffer And Add
    // Character By Character To A File To Reconstruct
    // Initial File
    getchar();
    read_manually(sem_crt, sem_clt, sem_rcstr, sharedData, buffer);
    
    // Destroy semaphores and detach from memory after finishing
    sem_close(sem_crt);
    sem_close(sem_clt);
    sem_close(sem_rcstr);

    detach_struct(sharedData);
    detach_buffer(buffer);
    return 0;
}