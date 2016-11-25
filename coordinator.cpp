#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>

unsigned int microseconds = 3000;


using namespace std;

int myrank, W, L, N;

int  PlateauToString(int Width, int Length, double **plateau){
	for (int i = 0; i < Width; i++)
	{
		for(int j = 0; j < Length; j++){
			printf("| %.3lf ", plateau[i][j] );
		}
		printf("|\n");
	}
}

int main( int argc, char *argv[] ) {
	double temperatureAmbiante;
	char endSignal = 'c';
	MPI_Comm parent;
	MPI_Status etat;
	MPI_Init (&argc, &argv);
	MPI_Comm_get_parent (&parent);
	MPI_Comm_rank (MPI_COMM_WORLD,&myrank);
	double **plateau;
	if (parent == MPI_COMM_NULL) {
		printf ("Fils %d : Coordinateur : Pas de pere !\n", myrank);
	}
	else {
		// Reception de la largeur du plateau
		MPI_Recv(&W, 1, MPI_INT, 0, 0, parent, &etat);

		// REception de la temperature ambiante
		MPI_Recv(&temperatureAmbiante, 1, MPI_DOUBLE, 0, 0, parent, &etat);
		printf ("Fils %d : Coordinateur : La temperature ambiante recue est de %.3lf !\n", myrank, temperatureAmbiante);

		// Reception de la longueur du plateau
		MPI_Recv(&L, 1, MPI_INT, 0, 0, parent, &etat);
		printf("Le Coordinateur a reçu la longueur %d du plateau\n", L );
		// Calcul du nombre d'esclave
		N = W*L;

		plateau = new double *[W];
		for(int i = 0; i < W; i++)
		    plateau[i] = new double[L];

		
		for (int j=0; j <10; j++){
			// Envoi de la temperature ambiante aux esclaves 10 fois
			for (int i=0; i < N; i++) {		
				printf ("Coordinateur envoie temp ambiante vers %d\n", (i+1));
				MPI_Send(&temperatureAmbiante, 1, MPI_DOUBLE, i+1, 0, MPI_COMM_WORLD);
			}

			// Pour chaques esclaves 
			//Attente de la reception de la température de tous les esclaves
			int nEsclave = 0;
			for (int i=0; i < W; i++) {	
				for (int j=0; j < L; j++) {	
					nEsclave++;
					double temperature;					
					//printf ("Coordinateur recoit ack de %d\n", nEsclave;
					MPI_Recv(&temperature, 1, MPI_DOUBLE, nEsclave, 0, MPI_COMM_WORLD, &etat);
					printf ("Coordinateur : La temperature de %d est de %.3lf !\n", nEsclave, temperature);
					plateau[i][j] = temperature;
				}
			}

			PlateauToString(W,L,plateau);
		}
		
		
		
		// Envoi du message de fin de simulation au maitre
		printf ("Fils %d : Coordinateur : Envoi vers le pere !\n", myrank);
		MPI_Send(&endSignal, 1, MPI_CHAR, 0, 0, parent);
		//usleep(microseconds);
	}

	MPI_Finalize();
	return 0;
}


