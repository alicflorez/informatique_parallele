#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

int myrank;

void  PlateauToString(int nbMacroLig, int nbMacroCol, int **plateau){
	
	printf("Fils %d : Affichage des températures voisines : \n", myrank );
	for (int i = 0; i < nbMacroLig; i++)
	{
		for(int j = 0; j < nbMacroCol; j++){
			printf("| %d ", plateau[i][j] );
		}

		printf("| \n", myrank);
	}
}

void PlateauDoubleToString(int nbMacroLig, int nbMacroCol, double **plateau){
	for (int i = 0; i < nbMacroLig; i++)
	{
		for(int j = 0; j < nbMacroCol; j++){
			printf("| %.3lf ", plateau[i][j] );
		}
		printf("|\n");
	}
}

int main( int argc, char *argv[] ) {

	// Propriétés
	double temperatureAmbiante;
	int nbMacroCol, nbMacroLig, nbMicroCol, nbMicroLig, positionCol, positionLig;
	char endSignal = 'e';
	int **macroVoisins;
	double **tempVoisins;
	double* microTemperatures;
	int **rangPlateau;

	// Initialisation communication
	MPI_Comm parent;
	MPI_Status etat;
	MPI_Init (&argc, &argv);
	MPI_Comm_get_parent (&parent);
	MPI_Comm_rank (MPI_COMM_WORLD,&myrank);
	MPI_Request req;


	
	if (parent == MPI_COMM_NULL) {
		printf ("Fils %d : Esclave : Pas de pere !\n", myrank);
	} else {

		// Reception de la largeur de la part du maitre
		MPI_Recv(&nbMacroCol, 1, MPI_INT, 0, 0, parent, &etat);
		//printf("Fils %d : Esclave a reçu nbMacroCol %d du plateau\n", myrank, nbMacroCol );

		// Reception de la longueur de la part du maitre
		MPI_Recv(&nbMacroLig, 1, MPI_INT, 0, 0, parent, &etat);
		//printf("Fils %d : Esclave a reçu nbMacroLig %d du plateau\n", myrank, nbMacroLig );

		// Reception de la largeur de la part du maitre
		MPI_Recv(&nbMicroCol, 1, MPI_INT, 0, 0, parent, &etat);
		//printf("Fils %d : Esclave a reçu nbMicroCol %d du plateau\n", myrank, nbMicroCol );

		// Reception de la longueur de la part du maitre
		MPI_Recv(&nbMicroLig, 1, MPI_INT, 0, 0, parent, &etat);
		printf("Fils %d : Esclave a reçu nbMicroLig %d du plateau\n", myrank, nbMicroLig );

       
		printf("Fils %d : Esclave creer un tableau de %dx%d du plateau\n", myrank, nbMicroLig , nbMicroCol);

		microTemperatures = new double [nbMicroCol*nbMicroLig];

		// Reception de la température de la dalle
		MPI_Recv(microTemperatures, nbMicroLig*nbMicroCol, MPI_DOUBLE, 0, 0, parent, &etat);
/*
		for (int i = 0; i < nbMicroLig; i++)
	     {
		for(int j = 0; j < nbMicroCol; j++){
			printf("| %6.3lf ", microTemperatures[i*nbMicroLig+j] );
		 }
		printf("|\n");
	     } 
		//printf ("Fils %d : Esclave : La temperature est de %.3lf !\n", myrank, microTemperatures[0]);

/*

		// Envoi du message de "bonne reception" au MAITRE
		MPI_Send(&endSignal, 1, MPI_CHAR, 0, 0, parent);
		//printf ("Fils %d : Esclave : Envoi vers le pere !\n", myrank);
		
		//Calcul de la position du carré sur le plateau
		positionCol = (myrank-1)%nbMacroCol;
		positionLig = (myrank-1)/nbMacroCol;

		macroVoisins = new int *[3];
		for(int i = 0; i < 3; i++){
		    macroVoisins[i] = new int[3];
		}

		// // Initialisation des macroVoisins à -1
		// for(int i = 0; i < 3; i++)
		// 	for(int j = 0; j < 3; j++)
		// 		macroVoisins[i][j] = -1;

		// Initialisation du tableau contenant le rang de tout les esclaves avec leur coordonnées
		rangPlateau = new int *[nbMacroLig];
		for(int i = 0; i < nbMacroLig; i++)
		    rangPlateau[i] = new int[nbMacroCol];
		
		for(int i = 0; i < nbMacroLig; i++)
			for(int j = 0; j < nbMacroCol; j++)
				rangPlateau[i][j] = i*nbMacroCol+j+1;

	
		
		
		//printf("Fils %d : Affichage des macroVoisins vide : \n", myrank );
		//PlateauToString(3,3, macroVoisins);
		
		for(int y=-1; y<=1; y++) {
			for(int x=-1; x<=1; x++) {
				if ((positionLig+y >= 0 && positionLig+y < nbMacroLig ) && (positionCol+x >= 0 && positionCol+x < nbMacroCol ))//rangPlateau[positionLig+y][positionCol+x]!=-1)
					macroVoisins[y+1][x+1] = rangPlateau[positionLig+y][positionCol+x];
				else 
					macroVoisins[y+1][x+1] = -1;
			}
		}

		for (int cicle=0; cicle<10; cicle++) {
			// Reception de la température ambiante de la part du coordinateur
			MPI_Recv(&temperatureAmbiante, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &etat);

			// TODO	: Ajouter Pragma
			for (int cursorMicroLig = 0; cursorMicroLig < nbMicroLig; cursorMicroLig++)
			{
				// TODO	: Ajouter Pragma
				for (int cursorMicroCol = 0; cursorMicroCol < nbMicroCol; cursorMicroCol++)
				{
					int **microVoisins;
					double **tempVoisins;
					microVoisins = new int *[3];
					tempVoisins = new double *[3];
					for(int tmpInit = 0; tmpInit < 3; tmpInit++){
					    microVoisins[tmpInit] = new int[3];
					    tempVoisins[tmpInit] = new double[3];
					}
					// Envoi de la température à tous les macroVoisins
					for(int i = 0; i < 3; i++)
						for(int j = 0; j < 3; j++) {
							
							for(int y=-1; y<=1; y++) {
								for(int x=-1; x<=1; x++) {
									if ((i+y >= 0 && i+y < nbMicroLig ) && (j+x >= 0 && j+x < nbMicroCol )) {//rangPlateau[positionLig+y][positionCol+x]!=-1)
										microVoisins[y+1][x+1] = myrank;
									} else {
										microVoisins[y+1][x+1] = macroVoisins[int((positionLig*nbMicroLig+(i+1)+y)/nbMicroLig)][int((positionCol*nbMicroCol+(j+1)+x)/nbMicroCol)];
										if (microVoisins[y+1][x+1] != -1) {
											MPI_Isend(&microTemperatures[i][j], 1, MPI_DOUBLE, microVoisins[y+1][x+1], 0, MPI_COMM_WORLD, &req);
										}
									}
								}
							}
						}
					//printf("Fils %d : Température envoyée à tous les macroVoisins !\n", myrank );
					
					// Reception de la temperature de la part des macroVoisins
					for(int i = 0; i < 3; i++) {
						for(int j = 0; j < 3; j++) {

							if (microVoisins[i][j] == -1) {
								tempVoisins[i][j] = temperatureAmbiante;
							} else if (microVoisins[i][j] != myrank) {								
								double temp;
								MPI_Recv(&temp, 1, MPI_DOUBLE, microVoisins[i][j], 0, MPI_COMM_WORLD, &etat);
								tempVoisins[i][j] = temp;
								//MPI_Request_free(&req);
								//printf("Fils %d : Envoi de la temperature au macroVoisins Fils %d \n", myrank, macroVoisins[i][j]);
							} else {
								tempVoisins[i][j] = microTemperatures[i][j];
							}
						}
					}

					// Calcul de la nouvelle température ( moyenne de toutes les température des macroVoisins + le carré)
					double sum=0;
					for(int i = 0; i < 3; i++)
						for(int j = 0; j < 3; j++)
							sum += tempVoisins[i][j];

					microTemperatures[cursorMicroLig][cursorMicroCol] = sum / 9 ;

					printf("Fils %d : L'Esclave a mis à jour sa temperature ! \n", myrank );
					//PlateauDoubleToString(3,3, tempVoisins);
					
					// Envoi du ACK au coordinateur
					MPI_Send(&microTemperatures, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
				}
			}
		}

		// Libération de l'espace mémoire
		for(int i = 0; i < 3; i++){
		    delete[] macroVoisins[i];
		    delete[] tempVoisins[i];
		}

		delete[] tempVoisins;
		delete[] macroVoisins;

		for(int i = 0; i < nbMacroLig; i++)
	    	delete[] rangPlateau[i];

	    delete[] rangPlateau;

	    */

	  //  delete[] microTemperatures;
	}

	printf ("Fils %d : Slave: Fin !\n", myrank);

	MPI_Finalize();
	return 0;
}
