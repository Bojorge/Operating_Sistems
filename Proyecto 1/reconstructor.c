#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h>

#include "shared_memory.h"

void read_manually(sem_t *sem_crt, sem_t *sem_clt, sem_t *sem_rcstr, SharedData *sharedData, Sentence *buffer) {
    char *dynamic_string = NULL;
    size_t buffer_size = 0;
    size_t length = 0;

    while (!sharedData->writingFinished) {
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

        char input;
        scanf(" %c", &input);

        if (input == '\n') {
            sem_wait(sem_rcstr);
            
            sem_wait(sem_var_read);

            int index = sharedData->readIndex;
            printf("Reading from buffer: \nbuffer[%d] = \"%c\" | time: %s\n", index, buffer[index].character, buffer[index].time);

            buffer_size++;
            char *temp = realloc(dynamic_string, buffer_size * sizeof(char));
            if (temp == NULL) {
                fprintf(stderr, "Error: No se pudo realocar la memoria\n");
                free(dynamic_string);
                exit(EXIT_FAILURE);
            }
            dynamic_string = temp;

            dynamic_string[length++] = buffer[index].character;
            dynamic_string[length] = '\0';

            printf("Resultado: %s\n\n", dynamic_string);

            buffer[index].character = '\0';
            strcpy(buffer[index].time, "");

            sharedData->charsTransferred++;
            sharedData->readIndex = (sharedData->readIndex + 1) % sharedData->bufferSize;

            sem_post(sem_var_write);

            sem_post(sem_clt);
            sem_post(sem_crt);
        }
    }

    free(dynamic_string);

}

void read_automatically(sem_t *sem_crt, sem_t *sem_clt, sem_t *sem_rcstr, SharedData *sharedData, Sentence *buffer, int interval) {
    char *dynamic_string = NULL;
    size_t buffer_size = 0;
    size_t length = 0;

    while (!sharedData->writingFinished) {
        sem_wait(sem_rcstr);

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

        // Read after checking if semaphore is open
        sem_wait(sem_var_read);

        int index = sharedData->readIndex;
        printf("Reading from buffer: \nbuffer[%d] = \"%c\" | time: %s\n", index, buffer[index].character, buffer[index].time);

        buffer_size++;
        char *temp = realloc(dynamic_string, buffer_size * sizeof(char));
        if (temp == NULL) {
            fprintf(stderr, "Error: No se pudo realocar la memoria\n");
            free(dynamic_string);
            exit(EXIT_FAILURE);
        }
        dynamic_string = temp;

        dynamic_string[length++] = buffer[index].character;
        dynamic_string[length] = '\0';

        printf("%s\n", dynamic_string);

        buffer[index].character = '\0';
        strcpy(buffer[index].time, "");

        sharedData->charsTransferred++;
        sharedData->readIndex = (sharedData->readIndex + 1) % sharedData->bufferSize;

        sem_post(sem_var_write);

        sem_post(sem_clt);
        sem_post(sem_crt);

        sleep(interval);
    }

    free(dynamic_string);

}

int main(int argc, char *argv[]) {
    // Verificar número de argumentos
    if (argc < 2 || argc > 3) {
        printf("Uso: %s <modo> [intervalo]s\n", argv[0]);
        printf("Modo: 0 = Manual, 1 = Automático\n");
        return -1;
    }

    int mode = atoi(argv[1]);
    int interval = 0;
    if (mode == 1 && argc == 3) {
        interval = atoi(argv[2]);
        if (interval <= 0) {
            printf("Intervalo no válido\n");
            return -1;
        }
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

    // Realizar llamadas a funciones según el modo especificado
    if (mode == 0) {
        read_manually(sem_crt, sem_clt, sem_rcstr, sharedData, buffer);
    } else if (mode == 1) {
        read_automatically(sem_crt, sem_clt, sem_rcstr, sharedData, buffer, interval);
    } else {
        printf("Modo no válido\n");
        return -1;
    }

    // Detach from memory after finishing
    detach_struct(sharedData);
    detach_buffer(buffer);

    return 0;
}
