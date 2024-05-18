
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main() {
	// Initialize the MPI environment
	MPI_Init(NULL, NULL);
    
	// Get the number of processors
	int num_p;
	MPI_Comm_size(MPI_COMM_WORLD, &num_p);
    
	// Get the rank of the processor
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
	// Get the name of the processor
	char nombre_proc[MPI_MAX_PROCESSOR_NAME];
	int longitud;
	MPI_Get_processor_name(nombre_proc, &longitud);
    
	// Print a message from each processor
	printf("Hola mundo desde el procesador %s, con rango %d de %d procesadores\n",
       	nombre_proc, rank, num_p);
    
	// Finalize the MPI environment
	MPI_Finalize();
    
	return 0;
}

