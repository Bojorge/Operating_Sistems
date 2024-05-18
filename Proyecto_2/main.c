#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size < 2) {
    	printf("Se necesitan al menos dos procesos para este ejemplo.\n");
    	MPI_Finalize();
    	return 1;
	}

	if (rank == 0) {
    	// Proceso maestro (rank 0)
    	char message[] = "Hola, Nodo 1";
    	MPI_Send(message, sizeof(message), MPI_CHAR, 1, 0, MPI_COMM_WORLD);
    	printf("Mensaje enviado desde el nodo %d.\n", rank);
	} else if (rank == 1) {
    	// Otro proceso (rank 1)
    	char message[100];
    	MPI_Recv(message, 100, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    	printf("Mensaje recibido en el nodo %d: %s\n", rank, message);
	}

	MPI_Finalize();
	return 0;
}

