#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>

unsigned int microseconds = 3000;


using namespace std;

int main( int argc, char *argv[] ) {
	int temperature, myrank;
	char endSignal = "c";
	MPI_Comm parent;
	MPI_Status etat;
	MPI_Init (&argc, &argv);
	MPI_Comm_get_parent (&parent);
	MPI_Comm_rank (MPI_COMM_WORLD,&myrank);

	if (parent == MPI_COMM_NULL) {
		printf ("Fils %d : Coordinateur : Pas de pere !\n", myrank);
	}
	else {
		MPI_Recv(&temperature, 1, MPI_INT, 0, 0, parent, &etat);
		printf ("Fils %d : Coordinateur : La temperature est de %d !\n", myrank, temperature);
		printf ("Fils %d : Coordinateur : Reception du pere !\n", myrank);
		MPI_Send(&endSignal, 1, MPI_CHAR, 0, 0, parent);
		printf ("Fils %d : Coordinateur : Envoi vers le pere !\n", myrank);
		
		usleep(microseconds);
	}

	MPI_Finalize();
	return 0;
}

