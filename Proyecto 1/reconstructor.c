#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared_memory.h"

bool wait_semaphores() {
    sem_t *sem_readIndex = sem_open(SEM_R_INDEX, 0);
    sem_t *sem_reconsBlock = sem_open(SEM_RB_TIME, 0);
    sem_t *sem_characterTransf = sem_open(SEM_C_TRANSF, 0);

    sem_wait(sem_readIndex);
    sem_wait(sem_reconsBlock);
    sem_wait(sem_characterTransf);

    return true;
}

void post_semaphores() {
    sem_t *sem_readIndex = sem_open(SEM_R_INDEX, 0);
    sem_t *sem_reconsBlock = sem_open(SEM_RB_TIME, 0);
    sem_t *sem_characterTransf = sem_open(SEM_C_TRANSF, 0);

    sem_post(sem_readIndex);
    sem_post(sem_reconsBlock);
    sem_post(sem_characterTransf);
}

void read_logic(SharedData *sharedData, Sentence *buffer) {
    char *dynamic_string = NULL;
    size_t buffer_size = 0;
    size_t length = 0;

    // Read the file
    FILE *file = fopen("reconstruido.txt", "r+");
    if (file == NULL) {
        printf("Error opening file %s\n", "reconstruido.txt");
    }

    // Leer el contenido completo del archivo y concatenarlo al string dinámico
    char character;
    while ((character = fgetc(file)) != EOF) {
        // Concatenar el caracter al final del string dinámico
        buffer_size++;
        char *temp = realloc(dynamic_string, buffer_size * sizeof(char));
        if (temp == NULL) {
            fprintf(stderr, "Error: No se pudo realocar la memoria\n");
            free(dynamic_string);
            exit(EXIT_FAILURE);
        }
        dynamic_string = temp;
        dynamic_string[length++] = character;
    }
    buffer_size ++;
    char *temp = realloc(dynamic_string, buffer_size * sizeof(char));
    dynamic_string = temp;

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

    // Print into console buffer index, character and time retreived
    int index = sharedData->readIndex;
    printf("Leyendo del buffer: \nbuffer[%d] = \"%c\" | tiempo: %s\n", index, buffer[index].character, buffer[index].time);

    dynamic_string[length] = buffer[index].character;
    printf("Archivo reconstruido: %s\n--------------------------\n", dynamic_string);

    // Add character to end of file
    fseek(file, 0, SEEK_END);
    fputc(buffer[index].character, file);

    // Borramos el caracter leido del buffer
    buffer[index].character = '\0';
    strcpy(buffer[index].time, "");
    
    // Actualizamos variables
    sharedData->charsTransferred++;
    sharedData->readIndex = (sharedData->readIndex + 1) % sharedData->bufferSize;
    sharedData->charsInBuffer--;

    // Cerramos archivo de reconstruccion
    fclose(file);

    // Posteamos los semaforos
    sem_post(sem_var_write);

    free(dynamic_string);
}

void read_manually(SharedData *sharedData, Sentence *buffer) {
    bool checkBreak = false;

    while (!sharedData->writingFinished) {
        char input[100];
        fgets(input, sizeof(input), stdin); // Lee la entrada del usuario

        // Comprueba si la entrada es "end"
        if (strcmp(input, "kill\n") == 0) {
            checkBreak = true;
            break;
        }

        // Comprueba si en alguna instancia se termino el proceso a la fuerza
        if (sharedData->reconsEndedProcess || sharedData->statsInited) {
            checkBreak = true;
            break;
        }

        // Esperar hasta que se presione enter y que los semaforos esten libres
        if (strcmp(input, "\n") == 0) {
            if (wait_semaphores()) {
                read_logic(sharedData, buffer);
                post_semaphores();
            }
        }
    }
    if (checkBreak) {
        printf("Terminando todas las instancias de reconstructor...\n");
        sharedData->reconsEndedProcess = true;
        post_semaphores();
    } else {
        sharedData->writingFinished = true;
        printf("El cliente ha enviado senal de terminado\n Terminando ejecucion...\n");;
    }
}

void read_automatically(SharedData *sharedData, Sentence *buffer, int interval) {
    bool checkBreak = false;

    while (!sharedData->writingFinished) {
        // Comprueba si en alguna instancia se termino el proceso a la fuerza
        if (sharedData->reconsEndedProcess || sharedData->statsInited) {
            checkBreak = true;
            break;
        }

        // Esperamos a ver si se puede leer
        if (wait_semaphores()) {
            read_logic(sharedData, buffer);
            post_semaphores();
        }

        // Esperar el intervalo especificado antes de la próxima lectura
        sleep(interval);
    }
    if (checkBreak) {
        printf("Terminando todas las instancias de reconstructor...\n");
        sharedData->reconsEndedProcess = true;
        post_semaphores();
    } else {
        sharedData->writingFinished = true;
        printf("El cliente ha enviado senal de terminado\n Terminando ejecucion...\n");;
    }
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

    // Connect to shared mem block
    SharedData *sharedData = attach_struct(STRUCT_LOCATION, sizeof(SharedData));
    if (sharedData == NULL) {
        printf("ERROR: no se pudo acceder al bloque\n");
        return -1;
    }
    sharedData->reconsEndedProcess = false;

    Sentence *buffer = attach_buffer(BUFFER_LOCATION, (sharedData->bufferSize * sizeof(Sentence)));
    if (buffer == NULL) {
        printf("ERROR: no se pudo acceder al bloque\n");
        return -1;
    }

    printf("escriba \"kill\" para acabar todas las instancias de reconstructor\n\n");

    // Realizar llamadas a funciones según el modo especificado
    if (mode == 0) {
        read_manually(sharedData, buffer);
    } else if (mode == 1) {
        read_automatically(sharedData, buffer, interval);
    } else {
        printf("Modo no válido\n");
        return -1;
    }

    // Detach from memory after finishing
    detach_struct(sharedData);
    detach_buffer(buffer);

    return 0;
}
