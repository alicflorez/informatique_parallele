#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;


int main( int argc, char *argv[] ) {
	
	char endSignal;
	int Y = 3, X = 3, y = 3, x = 3;
	double temperatureAmbiante = 20;

	if (argc > 2) {
		Y = atoi(argv[1]);
		X = atoi(argv[2]);
		y = atoi(argv[3]);
		x = atoi(argv[4]);
		temperatureAmbiante = atoi(argv[5]);
	} else {
		//N = 3;
		printf ("Pas d'arguments passés, N = %d.\n", X*Y);
	}

	int N = Y*X;
	int i;
	double**** temperature;

	temperature = new double ***[Y];
	for(int i = 0; i < Y; i++) {
	    temperature[i] = new double **[X];
	    for (int j = 0; j < X; j++) {
	    	temperature[i][j] = new double *[y];
	    	for (int k = 0; k < y; k++) {
	    		temperature[i][j][k] = new double[x];
			}		
		}		
	}		
		
	for(int i = 0; i < Y; i++)
	    for (int j = 0; j < X; j++)
	    	for (int k = 0; k < y; k++)
	    		for (int l = 0; l < y; l++)
	    			if ((l == x%2) && (k == y%2) && (j == X%2) && (i == Y%2))
	    				temperature[i][j][k][l] = 50;
	    			else
	    				temperature[i][j][k][l] = 30;

	MPI_Status etat;
	char *cmds[2] = {
		"coordinator.out", // Programme 1
		"slave.out" // Programme 2
	};

	int np[2];
	 np[0] = 1; // On lance 1 instances du coordinateur
	 np[1] = N; // On lance N instances du slave
	
	// Pas d'info supplémentaire pour contrôler le lancement
	// des programmes 1 et 2
	MPI_Info infos[2] = { MPI_INFO_NULL, MPI_INFO_NULL };
	int* errcodes = (int*) malloc ( sizeof (int) * (N+1)); // Les codes de retours des 5 processus
	MPI_Comm intercomm; // L'espace de communication père - fils
	MPI_Init( &argc, &argv );

	// On lance simultanément 2 instances de prg1 et 3 de prg2
	MPI_Comm_spawn_multiple (
		2,
		cmds,
		MPI_ARGVS_NULL,
		np,
		infos,
		0,
		MPI_COMM_WORLD,
		&intercomm,
		errcodes
	);

	printf ("Pere : J'ai lance toutes les instances.\n");

	// Le père communique de façon synchrone avec chacun de
	// ses fils en utilisant l'espace de communication intercomm
	for (int i=0; i < N+1; i++) {
		// Envoi la largeur du plateau à tous les fils
		MPI_Send (&X, 1, MPI_INT, i, 0, intercomm);
		// Envoi la longueur du plateau à tous les fils
		MPI_Send (&Y, 1, MPI_INT, i, 0, intercomm);

		// Envoi la largeur du plateau à tous les fils
		MPI_Send (&x, 1, MPI_INT, i, 0, intercomm);
		// Envoi la longueur du plateau à tous les fils
		MPI_Send (&y, 1, MPI_INT, i, 0, intercomm);

		if (i == 0) {
			//Envoi de la temperature ambiante
			MPI_Send (&temperatureAmbiante, 1, MPI_DOUBLE, i, 0, intercomm);
		}
		else {
					printf ("Pere : Envoi de %dx%d donnees vers %d.\n", x, y, i);

int kk=0;

					for (int ii = 0; ii < y; ii++)
	     {
		for(int jj = 0; jj < x; jj++){
			printf("| %6.3lf ", *(&(temperature[(i-1)%X][(i-1)/Y]) + kk) );
			kk++;
		 }
		printf("|\n");
	     }

			MPI_Send (&(temperature[(i-1)%X][(i-1)/Y]), x*y, MPI_DOUBLE, i, 0, intercomm);
		}
		

		printf ("\nPere : Envoi vers %d.\n", i);
	}

/*	for (i=0; i < N+1; i++) {
		MPI_Recv(&endSignal, 1, MPI_CHAR, i, 0, intercomm, &etat);
		printf ("Pere : Reception de %d.\n", i);
	}
*/
	printf ("Pere : Fin.\n");
	MPI_Finalize();
	return 0;
}	
