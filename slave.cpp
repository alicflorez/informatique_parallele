#include <mpi.h>
#include <stdio.h>
#include <iostream>

using namespace std;

int main( int argc, char *argv[] ) {

	// Propriétés
	double temperature, temperatureAmbiante;
	int myrank, nbCol, nbLig, positionLig, positionCol;
	char endSignal = 'e';
	int **voisins;
	int **rangPlateau;

	// Initialisation communication
	MPI_Comm parent;
	MPI_Status etat;
	MPI_Init (&argc, &argv);
	MPI_Comm_get_parent (&parent);
	MPI_Comm_rank (MPI_COMM_WORLD,&myrank);

	
	if (parent == MPI_COMM_NULL) {
		printf ("Fils %d : Esclave : Pas de pere !\n", myrank);
	}
	else {

		// Reception de la largeur de la part du maitre
		MPI_Recv(&nbCol, 1, MPI_INT, 0, 0, parent, &etat);
		printf("Fils %d : Esclave a reçu la longueur %d du plateau\n", myrank, nbCol );
		// Reception de la longueur de la part du maitre
		MPI_Recv(&nbLig, 1, MPI_INT, 0, 0, parent, &etat);
		printf("Fils %d : Esclave a reçu la longueur %d du plateau\n", myrank, nbCol );

		// Reception de la température de la dalle
		MPI_Recv(&temperature, 1, MPI_DOUBLE, 0, 0, parent, &etat);
		printf ("Fils %d : Esclave : La temperature est de %.3lf !\n", myrank, temperature);

		// Envoi du message de "bonne reception" au MAITRE
		MPI_Send(&endSignal, 1, MPI_CHAR, 0, 0, parent);
		printf ("Fils %d : Esclave : Envoi vers le pere !\n", myrank);
		
		//Calcul de la position du carré sur le plateau
		positionCol = (myrank-1)%nbCol;
		positionLig = (myrank-1)/nbCol;


		voisins = new int *[3];
		for(int i = 0; i < 3; i++)
		    voisins[i] = new int[3];

		for(int i = 0; i < 3; i++)
			for(int j = 0; j < 3; j++)
				voisins[i][j] = -1;

		rangPlateau = new int *[nbLig];
		for(int i = 0; i < nbLig; i++)
		    rangPlateau[i] = new int[nbCol];
		
		for(int i = 0; i < nbLig; i++)
			for(int j = 0; j < nbCol; j++)
				rangPlateau[i][j] = i*nbCol+j+1;

		if (positionLig > 0) {
			//on est pas dans y=0
		}
		if (positionLig < nbLig) {
			//on est pas dans y=max
		}
		if (positionCol > 0) {
			//on est pas dans x=0
		}
		if (positionCol < nbCol) {
			//on est pas dans x=max
		}
		// Angle Haut gauche
		if (positionLig == 0 && positionCol == 0) 
		{
			voisins[1][2] = rangPlateau[positionLig][positionCol+1];
			voisins[2][2] = rangPlateau[positionLig+1][positionCol+1];
			voisins[2][1] = rangPlateau[positionLig+1][positionCol];
		} 
		else if (positionLig == 0 && positionCol == nbCol-1) 
		{ // Angle Haut Droit
			voisins[1][0] = rangPlateau[0][positionCol-1];
			voisins[1][0] = rangPlateau[positionLig+1][positionCol-1];
			voisins[2][1] = rangPlateau[positionLig+1][positionCol];
		}
		else if (positionLig == 0 && positionCol == nbCol-1) 
		{ // Angle Haut Droit
			voisins[1][0] = rangPlateau[0][positionCol-1];
			voisins[1][0] = rangPlateau[positionLig+1][positionCol-1];
			voisins[2][1] = rangPlateau[positionLig+1][positionCol];
		}



		//for (int j=0; j <10; j++){
			// Reception de la température ambiante de la part du coordinateur
			MPI_Recv(&temperatureAmbiante, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &etat);
			printf ("Fils %d : Esclave : La temperature ambiante est de %.3lf !\n", myrank, temperatureAmbiante);

			// Envoi du ACK au coordinateur
			MPI_Send(&temperature, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		//}
	}

	MPI_Finalize();
	return 0;
}
