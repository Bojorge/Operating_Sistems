#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

#include "shared_memory.h"

bool wait_semaphores() {
    sem_t *sem_writeIndex = sem_open(SEM_W_INDEX, 0);
    sem_t *sem_readFileIndex = sem_open(SEM_RF_INDEX, 0);
    sem_t *sem_clientBlock = sem_open(SEM_CB_TIME, 0);
    sem_t *sem_characterBuffer = sem_open(SEM_C_BUFFER, 0);

    sem_wait(sem_writeIndex);
    sem_wait(sem_readFileIndex);
    sem_wait(sem_clientBlock);
    sem_wait(sem_characterBuffer);

    return true;
}

void post_semaphores() {
    sem_t *sem_writeIndex = sem_open(SEM_W_INDEX, 0);
    sem_t *sem_readFileIndex = sem_open(SEM_RF_INDEX, 0);
    sem_t *sem_clientBlock = sem_open(SEM_CB_TIME, 0);
    sem_t *sem_characterBuffer = sem_open(SEM_C_BUFFER, 0);

    sem_post(sem_writeIndex);
    sem_post(sem_readFileIndex);
    sem_post(sem_clientBlock);
    sem_post(sem_characterBuffer);
}

void insert_logic(FILE *file, SharedData *sharedData, Sentence *buffer) {
    char character;
    
    // Move the file pointer to the reading index
    fseek(file, sharedData->readingFileIndex, SEEK_SET);

    // Variables para leer restante de archivo
    char *dynamic_string = NULL;
    size_t buffer_size = 0;
    size_t length = 0;

    // Read the entire contents of the file and concatenate it to the dynamic string
    char character2;
    while ((character2 = fgetc(file)) != EOF) {
        // Concatenate the character to the end of the dynamic string
        buffer_size++;
        char *temp = realloc(dynamic_string, buffer_size * sizeof(char));
        if (temp == NULL) {
            fprintf(stderr, "Error: Failed to reallocate memory\n");
            free(dynamic_string);
            exit(EXIT_FAILURE);
        }
        dynamic_string = temp;
        dynamic_string[length++] = character2;
    }

    // Get semaphore for the writing space to check if writing is available
    char sem_write_name[MAX_LENGTH];
    sprintf(sem_write_name, "%s%d", SEM_WRITE_VARIABLE_FNAME, sharedData->writeIndex);

    sem_t *sem_var_write = sem_open(sem_write_name, 0);
    if (sem_var_write == SEM_FAILED) {
        perror("sem_open/variables");
        exit(EXIT_FAILURE);
    }

    // Get semaphore for the reading space to post after writing
    char sem_read_name[MAX_LENGTH];
    sprintf(sem_read_name, "%s%d", SEM_READ_VARIABLE_FNAME, sharedData->writeIndex);

    sem_t *sem_var_read = sem_open(sem_read_name, 0);
    if (sem_var_read == SEM_FAILED) {
        perror("sem_open/variables");
        exit(EXIT_FAILURE);
    }

    // Write after checking if semaphore is open
    sem_wait(sem_var_write);

    // Get current timestamp in the desired format
    time_t current_time;
    struct tm *timeinfo;
    time(&current_time);
    timeinfo = localtime(&current_time);
    strftime(buffer[sharedData->writeIndex].time, MAX_TIME_LENGTH, "%b %d %Y %H:%M:%S", timeinfo);

    // Move the file pointer to the reading index
    fseek(file, sharedData->readingFileIndex, SEEK_SET);

    // Get character from the file
    character = fgetc(file);

    // Assign the character to the buffer
    int index = sharedData->writeIndex;
    printf("Remaining to transfer: %s\n", dynamic_string);
    buffer[sharedData->writeIndex].character = character;
    printf("Adding to buffer:\nbuffer[%d] = \"%c\" | time: %s\n------------------------\n", index, buffer[index].character, buffer[index].time);

    // Update shared indices
    sharedData->writeIndex = (sharedData->writeIndex + 1) % sharedData->bufferSize;
    sharedData->readingFileIndex++;
    sharedData->charsInBuffer++;

    free(dynamic_string);

    // Post so that the variable can be read
    sem_post(sem_var_read);  
}

void insert_manually(FILE *file, SharedData *sharedData, Sentence *buffer) {
    // Obtener la longitud del archivo
    fseek(file, 0, SEEK_END);
    long file_length = ftell(file);
    rewind(file);
    clock_t start, end;

    bool checkBreak = false;

    while (ftell(file) < file_length) {

        char input[100];
        fgets(input, sizeof(input), stdin); // Lee la entrada del usuario

        // Comprueba si la entrada es "end"
        if (strcmp(input, "kill\n") == 0) {
            checkBreak = true;
            break;
        }

        // Comprueba si en alguna instancia se termino el proceso a la fuerza
        if (sharedData->clientEndedProcess || sharedData->statsInited) {
            checkBreak = true;
            break;
        }

        // Esperar hasta que se presione Enter y se tenga el semaforo libre para escribir
        if (strcmp(input, "\n") == 0) {
            if (wait_semaphores()) {
                start = clock();
                insert_logic(file, sharedData, buffer);
                
                post_semaphores();
            }
            end = clock();

            sharedData->clientBlockedTime += ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
        }
    }
    if (checkBreak) {
        printf("Terminando todas las instancias de cliente...\n");
        sharedData->clientEndedProcess = true;
        post_semaphores();
    } else {
        sharedData->writingFinished = true;
        printf("El cliente ha terminado de escribir los caracteres del archivo\n Terminando ejecucion...\n");
    }
}

void insert_automatically(FILE *file, int interval, SharedData *sharedData, Sentence *buffer) {
    char character;
    
    // Obtener la longitud del archivo
    fseek(file, 0, SEEK_END);
    long file_length = ftell(file);
    rewind(file);
    clock_t start, end;

    bool checkBreak = false;

    while (ftell(file) < file_length) { // Mientras no se llegue al final del archivo
        // Comprueba si en alguna instancia se termino el proceso a la fuerza
        if (sharedData->clientEndedProcess || sharedData->statsInited) {
            checkBreak = true;
            break;
        }

        // Esperamos a que todos los semaforos necesarios esten
        if (wait_semaphores()) {
            start = clock();
            insert_logic(file, sharedData, buffer);

            post_semaphores();
        }     
        end = clock();
        sharedData->clientBlockedTime += ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;

        sleep(interval);
    }
    if (checkBreak) {
        printf("Terminando todas las instancias de cliente...\n");
        sharedData->clientEndedProcess = true;
        post_semaphores();
    } else {
        sharedData->writingFinished = true;
        printf("El cliente ha terminado de escribir los caracteres del archivo\n Terminando ejecucion...\n");
    }
}

int main(int argc, char *argv[]) 
{   
    // Check for specified file and mode same as interval
    if (argc < 3 || argc > 4) {
        printf("Uso: %s <archivo> <modo> [intervalo]s\n", argv[0]);
        printf("Modo: 0 = Manual, 1 = Automático\n");
        return -1;
    }

    // Connect to shared mem struct
    SharedData *sharedData = attach_struct(STRUCT_LOCATION, sizeof(SharedData));
    if (sharedData == NULL) {
        printf("ERROR: no se pudo acceder al bloque\n");
        return -1;
    }
    sharedData->clientEndedProcess = false;

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
    
    printf("escriba \"kill\" para acabar todas las instancias de cliente\n\n");

    int mode = atoi(argv[2]);
    if (mode == 0) { // Modo manual
        insert_manually(file, sharedData, buffer);
    } else if (mode == 1) { // Modo automático
        if (argc == 4) { // Se requiere el intervalo
            int interval = atoi(argv[3]);
            if (interval <= 0) {
                printf("Intervalo no válido\n");
                return -1;
            }
            insert_automatically(file, interval, sharedData, buffer);
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