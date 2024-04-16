#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared_memory.h"

void insert_manually(FILE *file, sem_t *sem_crt, sem_t *sem_clt, sem_t *sem_rcstr, SharedData *sharedData, Sentence *buffer) {
    char character;
    while ((character = fgetc(file)) != EOF) {
        getchar(); // Esperar hasta que se presione Enter

        sem_wait(sem_clt);

        // Get semaphore for said writing space, to check if writing is available
        char sem_write_name[MAX_LENGTH];
        sprintf(sem_write_name, "%s%d", SEM_WRITE_VARIABLE_FNAME, sharedData->writeIndex);

        sem_t *sem_var_write = sem_open(sem_write_name, 0);
        if (sem_var_write == SEM_FAILED) {
            perror("sem_open/variables");
            exit(EXIT_FAILURE);
        }

        // Get semaphore for said reading space, to post after writing
        char sem_read_name[MAX_LENGTH];
        sprintf(sem_read_name, "%s%d", SEM_READ_VARIABLE_FNAME, sharedData->writeIndex);

        sem_t *sem_var_read = sem_open(sem_read_name, 0);
        if (sem_var_read == SEM_FAILED) {
            perror("sem_open/variables");
            exit(EXIT_FAILURE);
        }

        // Write after checking if semaphore is open
        sem_wait(sem_var_write);

        // Obtener la marca de tiempo actual en el formato deseado
        time_t current_time;
        struct tm *timeinfo;
        time(&current_time);
        timeinfo = localtime(&current_time);
        strftime(buffer[sharedData->writeIndex].time, MAX_TIME_LENGTH, "%b %d %Y %H:%M:%S", timeinfo);

        // Asignar el carácter al buffer
        printf("Agregando caracter: %c\n", character);
        buffer[sharedData->writeIndex].character = character;

        // Actualizar los índices compartidos
        sharedData->charsRemaining--;
        sharedData->writeIndex = (sharedData->writeIndex + 1) % sharedData->bufferSize;

        // Post so that variable could be read
        sem_post(sem_var_read);

        // Post all semaphores so that other processes take control of shared mem    
        sem_post(sem_crt);
        sem_post(sem_rcstr);
    }
    sharedData->writingFinished = true;
}

void insert_automatically(FILE *file, int interval, sem_t *sem_crt, sem_t *sem_clt, sem_t *sem_rcstr, SharedData *sharedData, Sentence *buffer) {
    char character;
    while ((character = fgetc(file)) != EOF) {
        sem_wait(sem_clt);

        // Get semaphore for said writing space, to check if writing is available
        char sem_write_name[MAX_LENGTH];
        sprintf(sem_write_name, "%s%d", SEM_WRITE_VARIABLE_FNAME, sharedData->writeIndex);

        sem_t *sem_var_write = sem_open(sem_write_name, 0);
        if (sem_var_write == SEM_FAILED) {
            perror("sem_open/variables");
            exit(EXIT_FAILURE);
        }

        // Get semaphore for said reading space, to post after writing
        char sem_read_name[MAX_LENGTH];
        sprintf(sem_read_name, "%s%d", SEM_READ_VARIABLE_FNAME, sharedData->writeIndex);

        sem_t *sem_var_read = sem_open(sem_read_name, 0);
        if (sem_var_read == SEM_FAILED) {
            perror("sem_open/variables");
            exit(EXIT_FAILURE);
        }

        // Write after checking if semaphore is open
        sem_wait(sem_var_write);

        // Obtener la marca de tiempo actual en el formato deseado
        time_t current_time;
        struct tm *timeinfo;
        time(&current_time);
        timeinfo = localtime(&current_time);
        strftime(buffer[sharedData->writeIndex].time, MAX_TIME_LENGTH, "%b %d %Y %H:%M:%S", timeinfo);

        // Asignar el carácter al buffer
        printf("Agregando caracter: %c\n", character);
        buffer[sharedData->writeIndex].character = character;

        // Actualizar los índices compartidos
        sharedData->charsTransferred++;
        sharedData->charsRemaining--;
        sharedData->writeIndex = (sharedData->writeIndex + 1) % sharedData->bufferSize;

        // Post so that variable could be read
        sem_post(sem_var_read);

        // Post all semaphores so that other processes take control of shared mem    
        sem_post(sem_crt);
        sem_post(sem_rcstr);

        sleep(interval); // Esperar el intervalo especificado
    }
    sharedData->writingFinished = true;
}

int main(int argc, char *argv[]) 
{   
    // Check for specified file and mode same as interval
    if (argc < 3 || argc > 4) {
        printf("Uso: %s <archivo> <modo> [intervalo]s\n", argv[0]);
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

    // Connect to shared mem struct
    SharedData *sharedData = attach_struct(STRUCT_LOCATION, sizeof(SharedData));
    if (sharedData == NULL) {
        printf("ERROR: no se pudo acceder al bloque\n");
        return -1;
    }

    // Connect to shared mem buffer
    Sentence *buffer = attach_buffer(BUFFER_LOCATION, (sharedData->bufferSize * sizeof(Sentence)));
    if (buffer == NULL) {
        printf("ERROR: no se pudo acceder al bloque\n");
        return -1;
    }

    // Read the file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error opening file %s\n",argv[1]);
        return -1;
    }
    
    int mode = atoi(argv[2]);
    if (mode == 0) { // Modo manual
        insert_manually(file, sem_crt, sem_clt, sem_rcstr, sharedData, buffer);
    } else if (mode == 1) { // Modo automático
        if (argc == 4) { // Se requiere el intervalo
            int interval = atoi(argv[3]);
            if (interval <= 0) {
                printf("Intervalo no válido\n");
                return -1;
            }
            insert_automatically(file, interval, sem_crt, sem_clt, sem_rcstr, sharedData, buffer);
        } else {
            printf("Intervalo no especificado\n");
            return -1;
        }
    } else {
        printf("Modo no válido\n");
        return -1;
    }

    // Detach from memory after finishing
    detach_struct(sharedData);
    detach_buffer(buffer);
    return 0;
}