#include <stdio.h>
#include <mpi.h>
#include <string.h>

#define MSG_TAG 0
#define TERMINATE_TAG 1

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 1) {
        if (rank == 0) {
            printf("Se necesitan al menos cuatro procesos para este ejemplo (1 maestro y 3 esclavos).\n");
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        // Proceso maestro (rank 0)
        char message[] = "Hola, Nodo Esclavo";
        for (int i = 1; i < 4; i++) {
            MPI_Send(message, strlen(message) + 1, MPI_CHAR, i, MSG_TAG, MPI_COMM_WORLD);
            printf("Mensaje enviado desde el maestro al nodo %d.\n", i);
        }

        // Enviar señal de terminación a los esclavos
        for (int i = 1; i < 4; i++) {
            MPI_Send(NULL, 0, MPI_CHAR, i, TERMINATE_TAG, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
