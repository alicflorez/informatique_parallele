#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;


int main( int argc, char *argv[] ) {
	
	char endSignal;
	int W = 4, L=3;

	if (argc > 2) {
		W = atoi(argv[1]);
		L = atoi(argv[2]);
	} else {
		//N = 3;
		printf ("Pas d'arguments passés, N = 3.\n");
	}

	int N = L*W;

	int i;
	double temperature;
	MPI_Status etat;
	char *cmds[2] = {
		"coordinator.out", // Programme 1
		"slave.out" // Programme 2
	};

	int np[2];
	 np[0] = 1; // On lance 2 instances du programme 1
	 np[1] = N; // On lance N instances du programme 2
	
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
	for (i=0; i<N+1; i++) {
		if (i==0) {
			// Envoi la largeur du plateau au coordinateur
			MPI_Send (&W,1,MPI_INT,i,0,intercomm);

			temperature = 20;
		}
		else if (i==7)
			temperature = 50;
		else 
			temperature = 30;

		MPI_Send (&temperature,1,MPI_DOUBLE,i,0,intercomm);

		// Envoi la longueur du plateau au coordinateur
		MPI_Send (&L,1,MPI_INT,i,0,intercomm);

		printf ("\nPere : Envoi vers %d.\n", i);
		
	}

	for (i=0; i<N+1; i++) {
		MPI_Recv(&endSignal, 1, MPI_CHAR,i, 0, intercomm, &etat);
		printf ("Pere : Reception de %d.\n", i);
	}

	printf ("Pere : Fin.\n");
	MPI_Finalize();
	return 0;
}	
