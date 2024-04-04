#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define MAX_WORD_LENGTH 100
#define MAX_THREADS 5

// Estructura para pasar argumentos a la función de hilo
typedef struct {
    FILE *archivo;
    const char *palabra;
    long inicio;
    long fin;
    int *contador;
} ThreadArgs;

// Función para contar la cantidad de apariciones de una palabra en una parte del archivo
void *contarApariciones(void *args) {
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    FILE *archivo = threadArgs->archivo;
    const char *palabra = threadArgs->palabra;
    long inicio = threadArgs->inicio;
    long fin = threadArgs->fin;
    int *contador = threadArgs->contador;

    char buffer[MAX_WORD_LENGTH];

    // Ubica el puntero del archivo al inicio del segmento asignado al hilo
    fseek(archivo, inicio, SEEK_SET);

    // Realiza el conteo de palabras en el segmento del archivo
    while (ftell(archivo) < fin && fscanf(archivo, "%s", buffer) != EOF) {
        if (strcmp(buffer, palabra) == 0) {
            (*contador)++;
        }
    }

    pthread_exit(NULL);
}

// Función principal
int main() {
    const char *nombreArchivo = "texto.txt"; 
    const char *palabraBuscar = "lado";  
    pthread_t threads[MAX_THREADS];
    ThreadArgs threadArgs[MAX_THREADS];
    FILE *archivo;
    long fileSize, chunkSize, chunkStart, chunkEnd;
    int apariciones = 0;
    clock_t start, end;
    double time_spent;

    // Abre el archivo para lectura
    archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        fprintf(stderr, "No se pudo abrir el archivo %s\n", nombreArchivo);
        return 1;
    }

    // Obtiene el tamaño total del archivo
    fseek(archivo, 0, SEEK_END);
    fileSize = ftell(archivo);
    rewind(archivo);

    // Divide el archivo en segmentos para cada hilo
    chunkSize = fileSize / MAX_THREADS;

    // Inicia el temporizador
    start = clock();

    // Crea y ejecuta los hilos
    for (int i = 0; i < MAX_THREADS; i++) {
        chunkStart = i * chunkSize;
        chunkEnd = (i == MAX_THREADS - 1) ? fileSize : (i + 1) * chunkSize;
        threadArgs[i].archivo = archivo;
        threadArgs[i].palabra = palabraBuscar;
        threadArgs[i].inicio = chunkStart;
        threadArgs[i].fin = chunkEnd;
        threadArgs[i].contador = &apariciones;
        pthread_create(&threads[i], NULL, contarApariciones, (void *)&threadArgs[i]);
    }

    // Espera a que todos los hilos terminen
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Detiene el temporizador
    end = clock();

    // Calcula el tiempo transcurrido
    time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    // Cierra el archivo
    fclose(archivo);

    // Imprime el resultado y el tiempo de ejecución
    printf("La palabra '%s' aparece %d veces en el archivo '%s'\n", palabraBuscar, apariciones, nombreArchivo);
    printf("Tiempo de ejecución: %.6f segundos\n", time_spent);

    return 0;
}
