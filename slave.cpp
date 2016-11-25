#include <mpi.h>
#include <stdio.h>
#include <iostream>

using namespace std;

int main( int argc, char *argv[] ) {
	double temperature, temperatureAmbiante;
	int myrank, L;
	char endSignal = 'e';
	MPI_Comm parent;
	MPI_Status etat;
	MPI_Init (&argc, &argv);
	MPI_Comm_get_parent (&parent);
	MPI_Comm_rank (MPI_COMM_WORLD,&myrank);

	
	if (parent == MPI_COMM_NULL) {
		printf ("Fils %d : Esclave : Pas de pere !\n", myrank);
	}
	else {
		// Reception de la température de la dalle
		MPI_Recv(&temperature, 1, MPI_DOUBLE, 0, 0, parent, &etat);
		printf ("Fils %d : Esclave : La temperature est de %.3lf !\n", myrank, temperature);

		// Reception de la longueur de la part du maitre
		MPI_Recv(&L, 1, MPI_INT, 0, 0, parent, &etat);
		printf("Fils %d : Esclave a reçu la longueur %d du plateau\n", myrank, L );

		// Envoi du message de "bonne reception" au MAITRE
		MPI_Send(&endSignal, 1, MPI_CHAR, 0, 0, parent);
		printf ("Fils %d : Esclave : Envoi vers le pere !\n", myrank);
	

		for (int j=0; j <10; j++){
			// Reception de la température ambiante de la part du coordinateur
			MPI_Recv(&temperatureAmbiante, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &etat);
			printf ("Fils %d : Esclave : La temperature ambiante est de %.3lf !\n", myrank, temperatureAmbiante);

			// Envoi du ACK au coordinateur
			MPI_Send(&temperature, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		}
	}

	MPI_Finalize();
	return 0;
}
