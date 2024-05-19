#include <stdio.h>
#include <mpi.h>

#define MSG_TAG 0
#define TERMINATE_TAG 1

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 4) {
        if (rank == 0) {
            printf("Se necesitan al menos cuatro procesos para este ejemplo (1 maestro y 3 esclavos).\n");
        }
        MPI_Finalize();
        return 1;
    }

    if (rank != 0) {
        while (1) {
            MPI_Status status;
            MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == TERMINATE_TAG) {
                break;
            }

            char message[100];
            MPI_Recv(message, 100, MPI_CHAR, 0, MSG_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Mensaje recibido en el nodo %d: %s\n", rank, message);
        }
    }

    MPI_Finalize();
    return 0;
}
