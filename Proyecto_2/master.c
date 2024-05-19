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

    if (rank == 0) {
        printf("Maestro iniciado.\n");
        if (size < 2) {
            printf("Este programa necesita al menos un esclavo para funcionar correctamente.\n");
            MPI_Finalize();
            return 1;
        }

        char message[] = "Comenzar envio de mensajes";
        printf("Presiona Enter para comenzar a enviar mensajes a los esclavos...\n");
        getchar(); // Espera a que el usuario presione Enter
        for (int i = 1; i < size; i++) {
            MPI_Send(message, strlen(message) + 1, MPI_CHAR, i, MSG_TAG, MPI_COMM_WORLD);
            printf("Mensaje enviado al esclavo %d.\n", i);
        }

        // Enviar señal de terminación a los esclavos
        for (int i = 1; i < size; i++) {
            MPI_Send(NULL, 0, MPI_CHAR, i, TERMINATE_TAG, MPI_COMM_WORLD);
        }
    } else {
        while (1) {
            MPI_Status status;
            MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == TERMINATE_TAG) {
                break;
            }

            char message[100];
            MPI_Recv(message, 100, MPI_CHAR, 0, MSG_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Mensaje recibido en el esclavo %d: %s\n", rank, message);
        }
    }

    MPI_Finalize();
    return 0;
}
