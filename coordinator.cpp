#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>

unsigned int microseconds = 3000;

using namespace std;

int  PlateauToString(int nbLig, int nbCol, double **plateau){
	for (int i = 0; i < nbLig; i++)
	{
		for(int j = 0; j < nbCol; j++){
			printf("| %.3lf ", plateau[i][j] );
		}
		printf("|\n");
	}
}

int main( int argc, char *argv[] ) {
	// Propriétés
	double temperatureAmbiante;
	char endSignal = 'c';
	int myrank, N, nbLig, nbCol;
	double **plateau;

	// Initialisation communication
	MPI_Comm parent;
	MPI_Status etat;
	MPI_Init (&argc, &argv);
	MPI_Comm_get_parent (&parent);
	MPI_Comm_rank (MPI_COMM_WORLD,&myrank);

	if (parent == MPI_COMM_NULL) {
		printf ("Fils %d : Coordinateur : Pas de pere !\n", myrank);
	}
	else {
		// Reception de la largeur du plateau
		MPI_Recv(&nbCol, 1, MPI_INT, 0, 0, parent, &etat);
		// Reception de la longueur du plateau
		MPI_Recv(&nbLig, 1, MPI_INT, 0, 0, parent, &etat);
		//printf("Le Coordinateur a reçu la longueur %d du plateau\n", nbLig);
		// Calcul du nombre d'esclave
		N = nbCol*nbLig;

		// Reception de la temperature ambiante
		MPI_Recv(&temperatureAmbiante, 1, MPI_DOUBLE, 0, 0, parent, &etat);
		//printf ("Fils %d : Coordinateur : La temperature ambiante recue est de %.3lf !\n", myrank, temperatureAmbiante);


		plateau = new double *[nbLig];
		for(int i = 0; i < nbLig; i++)
		    plateau[i] = new double[nbCol];

		
		for (int j=0; j <10; j++){
			// Envoi de la temperature ambiante aux esclaves 10 fois
			for (int i=0; i < N; i++) {		
				//printf ("Coordinateur envoie temp ambiante vers %d\n", (i+1));
				MPI_Send(&temperatureAmbiante, 1, MPI_DOUBLE, i+1, 0, MPI_COMM_WORLD);
			}

			// Pour chaques esclaves 
			//Attente de la reception de la température de tous les esclaves
			int nEsclave = 0;
			for (int i=0; i < nbLig; i++) {	
				for (int j=0; j < nbCol; j++) {	
					nEsclave++;
					double temperature;					
					//printf ("Coordinateur recoit ack de %d\n", nEsclave;
					MPI_Recv(&temperature, 1, MPI_DOUBLE, nEsclave, 0, MPI_COMM_WORLD, &etat);
					printf ("Coordinateur : La temperature de %d est de %.3lf !\n", nEsclave, temperature);
					plateau[i][j] = temperature;
				}
			}

			PlateauToString(nbLig,nbCol,plateau);
		}
				
		// Envoi du message de fin de simulation au maitre
		printf ("Fils %d : Coordinateur : Envoi vers le pere !\n", myrank);
		MPI_Send(&endSignal, 1, MPI_CHAR, 0, 0, parent);


		for(int i = 0; i < nbLig; i++)
		    delete[] plateau[i];

		delete[] plateau;

		//usleep(microseconds);*/
	}
	printf ("Fils %d : Coordinateur : fin !\n", myrank);

	MPI_Finalize();
	return 0;
}