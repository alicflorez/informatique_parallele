#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <omp.h>
/*#include <sys/time.h>
#include <sys/resource.h>*/

using namespace std;

int myrank;

void  PlateauToString(int nbLig, int nbCol, int **plateau){
	
	printf("Fils %d : Affichage des températures voisines : \n", myrank );
	for (int i = 0; i < nbLig; i++)
	{
		for(int j = 0; j < nbCol; j++){
			printf("| %d ", plateau[i][j] );
		}

		printf("| \n", myrank);
	}
}

void PlateauDoubleToString(int nbLig, int nbCol, double **plateau){
	for (int i = 0; i < nbLig; i++)
	{
		for(int j = 0; j < nbCol; j++){
			printf("| %.3lf ", plateau[i][j] );
		}
		printf("|\n");
	}
}

int main( int argc, char *argv[] ) {

	/*struct rusage ru1, ru2;
	getrusage(RUSAGE_SELF, &ru1);*/

	int nbLig = 3;
	int nbCol = 3;

	double temperatureAmbiante = 20.0;

	double **tableauDepart = new double *[nbLig+2];
	double **tableauResultat = new double *[nbLig];

	for(int i = 0; i < nbLig+2; i++){
	    tableauDepart[i] = new double[nbCol+2];
	    tableauResultat[i] = new double[nbCol];
	}


	for(int i = 0; i<nbLig+2; i++)
			tableauDepart[i][0] = 50;
	for(int i = 0; i<nbLig+2; i++)
			tableauDepart[i][nbCol+1] = temperatureAmbiante;

	for(int i = 0; i<nbCol+2; i++)
			tableauDepart[0][i] = 50;
	for(int i = 0; i<nbCol+2; i++)
			tableauDepart[nbLig+1][i] = temperatureAmbiante;

	for(int i = 1; i <nbLig+1; i++)
		for (int j = 1; j < nbCol+1; j++)
			tableauDepart[i][j] = 30;


	PlateauDoubleToString(nbLig+2, nbCol+2, tableauDepart);

	omp_set_nested(1);
	#pragma omp parallel for num_threads (nbLig)
		for (int positionLig = 0; positionLig < nbLig; positionLig++)
		{
			#pragma omp parallel for num_threads (nbCol)
				for (int positionCol = 0; positionCol < nbCol; positionCol++)
				{

					double **voisinsTemperatures = new double *[3];
					for(int i = 0; i < 3; i++)
					    voisinsTemperatures[i] = new double[3];

					for(int y=-1; y<=1; y++) {
						for(int x=-1; x<=1; x++) {
							if ((positionLig+y >= 0 && positionLig+y < nbLig ) && (positionCol+x >= 0 && positionCol+x < nbCol ))//rangPlateau[positionLig+y][positionCol+x]!=-1)
								voisinsTemperatures[y+1][x+1] = tableauDepart[positionLig+y][positionCol+x];
							else
								voisinsTemperatures[y+1][x+1] = temperatureAmbiante;
						}
					}

					int sum = 0;
					for (int i = 0; i < 3; i++)
						for (int j = 0; j < 3; j++){ 
							sum += voisinsTemperatures[i][j]; 
						}

					tableauDepart[positionLig][positionCol] = sum / 9;

				}
		}
	

	PlateauDoubleToString(nbLig, nbCol, tableauDepart);

	//getrusage(RUSAGE_SELF, &ru2);

/*
	// Propriétés
	double temperature, temperatureAmbiante;
	int nbCol, nbLig, positionLig, positionCol;
	char endSignal = 'e';
	int **voisins;
	double **tempVoisins;
	int **rangPlateau;


		//Calcul de la position du carré sur le plateau
		positionCol = (myrank-1)%nbCol;
		positionLig = (myrank-1)/nbCol;

		tempVoisins = new double *[3];
		voisins = new int *[3];
		for(int i = 0; i < 3; i++){
		    voisins[i] = new int[3];
		    tempVoisins[i] = new double[3];
		}

		for(int i = 0; i < 3; i++)
			for(int j = 0; j < 3; j++)
				voisins[i][j] = -1;

		rangPlateau = new int *[nbLig];
		for(int i = 0; i < nbLig; i++)
		    rangPlateau[i] = new int[nbCol];
		
		for(int i = 0; i < nbLig; i++)
			for(int j = 0; j < nbCol; j++)
				rangPlateau[i][j] = i*nbCol+j+1;

		
		
		//printf("Fils %d : Affichage des voisins vide : \n", myrank );
		//PlateauToString(3,3, voisins);
		
		for(int y=-1; y<=1; y++) {
			for(int x=-1; x<=1; x++) {
				if ((positionLig+y >= 0 && positionLig+y < nbLig ) && (positionCol+x >= 0 && positionCol+x < nbCol ))//rangPlateau[positionLig+y][positionCol+x]!=-1)
					voisins[y+1][x+1] = rangPlateau[positionLig+y][positionCol+x];
			}
		}

		for (int j=0; j <10; j++) {
			// Reception de la température ambiante de la part du coordinateur
			MPI_Recv(&temperatureAmbiante, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &etat);
			//printf ("Fils %d : Esclave : La temperature ambiante est de %.3lf !\n", myrank, temperatureAmbiante);

			// Envoi de la température à tous les voisins
			for(int i = 0; i < 3; i++)
				for(int j = 0; j < 3; j++) {
					if (voisins[i][j] != -1 && voisins[i][j] != myrank) {
						MPI_Isend(&temperature, 1, MPI_DOUBLE, voisins[i][j], 0, MPI_COMM_WORLD, &req);
						//MPI_Request_free(&req);
						//printf("Fils %d : Envoi de la temperature au voisins Fils %d \n", myrank, voisins[i][j]);
					}
				}
			//printf("Fils %d : Température envoyée à tous les voisins !\n", myrank );
			
			// Reception de la temperature de la part des voisins
			for(int i = 0; i < 3; i++) {
				for(int j = 0; j < 3; j++) {
					if (voisins[i][j] != myrank) {
						if (voisins[i][j] != -1 ) {
							double temp;
							MPI_Recv(&temp, 1, MPI_DOUBLE, voisins[i][j], 0, MPI_COMM_WORLD, &etat);
							tempVoisins[i][j] = temp;
							//printf("Fils %d : Température reçu de la part de Slave %d ! \n", myrank, voisins[i][j] );
						} else 
							tempVoisins[i][j] = temperatureAmbiante;						
					} else
						tempVoisins[i][j] = temperature;					
				}
			}

			// Calcul de la nouvelle température ( moyenne de toutes les température des voisins + le carré)
			double sum=0;
			for(int i = 0; i < 3; i++)
				for(int j = 0; j < 3; j++)
					sum += tempVoisins[i][j];

			temperature = sum / 9 ;

			printf("Fils %d : L'Esclave a mis à jour sa temperature ! \n", myrank );
			//PlateauDoubleToString(3,3, tempVoisins);

		}

		// Libération de l'espace mémoire
		for(int i = 0; i < 3; i++){
		    delete[] voisins[i];
		    delete[] tempVoisins[i];
		}

		delete[] tempVoisins;
		delete[] voisins;

		for(int i = 0; i < nbLig; i++)
	    	delete[] rangPlateau[i];

	    delete[] rangPlateau;
	}

	printf ("Fils %d : Slave: Fin !\n", myrank);
*/
	return 0;
}
