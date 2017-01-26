#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "Plaque_Metal.h"

using namespace std;


int main( int argc, char *argv[] ) {
    char endSignal;
    char*  filename;
    int nbCycles;
    int tailleMatriceCase;
    double temperatureAmbiante;
    if (argc > 4) {
        filename = argv[1];
        nbCycles = atoi(argv[2]);
        tailleMatriceCase = atoi(argv[3]);
        temperatureAmbiante = atof(argv[4]);
    } else {
        printf ("Pas assez d'arguments passés: nom fichier / nombre de cycles de refroidissement / taille de la matrice de la case du plateau / temperature ambiante.\n");
        exit(EXIT_FAILURE);
    }
    
    Plaque_Metal plateau(filename, tailleMatriceCase); 
    int nbLig=plateau.getNbLignes(), nbCol=plateau.getNbColonnes();
    
    
    int N = nbLig*nbCol;
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
    
    #pragma omp parallel for num_threads (N+1)
    for (int i=0; i< N+1; i++) {
        
        // Envoi la largeur du plateau à tous les fils
        MPI_Send (&nbCol, 1, MPI_INT, i, 0, intercomm);
        // Envoi la longueur du plateau à tous les fils
        MPI_Send (&nbLig, 1, MPI_INT, i, 0, intercomm);

        // Envoi la largeur de la matrice de la case à tous les fils
        MPI_Send (&tailleMatriceCase, 1, MPI_INT, i, 0, intercomm);
        
        MPI_Send (&nbCycles, 1, MPI_INT, i, 0, intercomm);
        
        
        if (i == 0)
            MPI_Send (&temperatureAmbiante, 1, MPI_DOUBLE, i, 0, intercomm);
        else {
            double *caseEsclave=plateau.getCaseByRank(i-1);
            MPI_Send (caseEsclave, tailleMatriceCase*tailleMatriceCase, MPI_DOUBLE, i, 0, intercomm);
//            double caseEsclave=plateau.getAverageByRank(i-1);
//            MPI_Send (&caseEsclave, 1, MPI_DOUBLE, i, 0, intercomm);
        }
        printf ("\nPere : Envoi vers %d.\n", i);
    }

    for (int i=0; i < N+1; i++) {
        MPI_Recv(&endSignal, 1, MPI_CHAR,i, 0, intercomm, &etat);
        printf ("Pere : Reception de %d.\n", i);
    }

    printf ("Pere : Fin.\n");
    MPI_Finalize();
    return 0;
}	
