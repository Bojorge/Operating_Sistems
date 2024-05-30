#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Texto a enviar desde el proceso maestro
    const char* text = "Este es el texto enviado desde el maestro a los esclavos";
    char* receivedText = NULL;
    int textLength = 0;

    if (rank == 0) {
        // Proceso maestro
        textLength = strlen(text) + 1; // Incluye el carácter nulo al final
    }

    // Broadcast para enviar la longitud del texto a todos los procesos
    MPI_Bcast(&textLength, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Asignar memoria para recibir el texto en cada proceso esclavo
    receivedText = (char*)malloc(textLength * sizeof(char));

    if (rank == 0) {
        // Proceso maestro envía el texto
        MPI_Bcast((void*)text, textLength, MPI_CHAR, 0, MPI_COMM_WORLD);
    } else {
        // Procesos esclavos reciben el texto
        MPI_Bcast(receivedText, textLength, MPI_CHAR, 0, MPI_COMM_WORLD);
    }

    // Cada proceso imprime el texto recibido
    printf("Proceso %d recibió el texto: %s\n", rank, receivedText);

    // Liberar memoria asignada
    free(receivedText);

    MPI_Finalize();
    return 0;
}
