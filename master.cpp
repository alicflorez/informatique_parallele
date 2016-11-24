#include <mpi.h>
#include <stdio.h>

int main( int argc, char *argv[] ) {
	int i, compteur;
	MPI_Status etat;
	char *cmds[2] = {
		"slave.out",
		"coordinator.out"
	};

	int np[2] = {
		12, // On lance 2 instances du programme 1
		1 // On lance 3 instances du programme 2
	};

	// Pas d'info supplémentaire pour contrôler le lancement
	// des programmes 1 et 2
	MPI_Info infos[2] = { MPI_INFO_NULL, MPI_INFO_NULL };
	int errcodes[5]; // Les codes de retours des 5 processus
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
	for (i=0; i<5; i++) {
		MPI_Send (&compteur,1,MPI_INT,i,0,intercomm);
		printf ("Pere : Envoi vers %d.\n", i);
		MPI_Recv(&compteur, 1, MPI_INT,i, 0, intercomm, &etat);
		printf ("Pere : Reception de %d.\n", i);
	}

	printf ("Pere : Fin.\n");
	MPI_Finalize();
	return 0;
}	
