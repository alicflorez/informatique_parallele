#include <mpi.h>
#include <stdio.h>
//#include <iostream>
//#include <unistd.h>
//#include <stdlib.h>
//#include <string.h>
//#include <math.h>
#include "IHM.h"
#include "Plaque_Metal.h"


using namespace std;

int main( int argc, char *argv[] ) {
    // Propriétés
    double temperatureAmbiante;
    char endSignal = 'c';
    int myrank, N, nbLig, nbCol, tailleCoteCase, nbCycles, vitesse;

    // Initialisation communication
    MPI_Comm parent;
    MPI_Status etat;
    MPI_Init(&argc, &argv);
    MPI_Comm_get_parent(&parent);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    // IHM 
    if (parent == MPI_COMM_NULL) {
        printf ("Fils %d : Coordinateur : Pas de pere !\n", myrank);
    } else {
        // Reception de la largeur du plateau
        MPI_Recv(&nbCol, 1, MPI_INT, 0, 0, parent, &etat);
        // Reception de la longueur du plateau
        MPI_Recv(&nbLig, 1, MPI_INT, 0, 0, parent, &etat);
        // Reception de la longueur du plateau
        MPI_Recv(&tailleCoteCase, 1, MPI_INT, 0, 0, parent, &etat);
        //printf("Le Coordinateur a reçu la longueur %d du plateau\n", nbLig);

        MPI_Recv(&nbCycles, 1, MPI_INT, 0, 0, parent, &etat);

        //FenetreGrahique_dessinerRectangle (-(0.95/(nbCol)), -(0.95/(nbLig)), (0.95/(nbCol/2)), (0.95/(nbLig/2)), 1, 0, 1, 0, 1, 0);
        // Calcul du nombre d'esclave
        N = nbCol*nbLig;

        // Reception de la temperature ambiante
        MPI_Recv(&temperatureAmbiante, 1, MPI_DOUBLE, 0, 0, parent, &etat);
        MPI_Recv(&vitesse, 1, MPI_INT, 0, 0, parent, &etat);
        //printf ("Fils %d : Coordinateur : La temperature ambiante recue est de %.3lf !\n", myrank, temperatureAmbiante);


        Plaque_Metal plateau = Plaque_Metal(nbLig, nbCol, tailleCoteCase);

        // Envoi de la temperature ambiante aux esclaves 10 fois
        for (int i=1; i<=N; i++) {		
            //printf ("Coordinateur envoie temp ambiante vers %d\n", (i+1));
            MPI_Send(&temperatureAmbiante, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
            
        
        int nEsclave = 0;
        for (int i=0; i < nbLig; i++) {	
            for (int j=0; j < nbCol; j++) {	
                nEsclave++;
                double temperature[tailleCoteCase*tailleCoteCase];
                MPI_Recv(temperature, tailleCoteCase*tailleCoteCase, MPI_DOUBLE, nEsclave, 0, MPI_COMM_WORLD, &etat);
                
                printf ("Coordinateur : La temperature de %d est de %.3lf !\n", nEsclave, temperature[0]); //TODO

                plateau.set(nEsclave, temperature);
            }
        }
            
        plateau.printAll();
        plateau.findMaxTemperature();


        IHM ihm = IHM(800, 600, -2, 2, -2, 2, vitesse);

        ihm.FenetreGraphique_rendre();
        ihm.FenetreGraphique_initialiser();
        ihm.FenetreGraphique_representer_plaque(plateau);
        ihm.FenetreGraphique_rendre();        
        
        for (int iteration=0; iteration<nbCycles; iteration++){

            // Pour chaques esclaves 
            //Attente de la reception de la température de tous les esclaves
            nEsclave = 0;

            for (int i=0; i<nbLig; i++) {	
                for (int j=0; j<nbCol; j++) {	
                    nEsclave++;
                    double temperature[tailleCoteCase*tailleCoteCase];
                    MPI_Recv(temperature, tailleCoteCase*tailleCoteCase, MPI_DOUBLE, nEsclave, 0, MPI_COMM_WORLD, &etat);
                    
                    printf ("Coordinateur : La temperature de %d est de %.3lf !\n", nEsclave, temperature[0]);

                    plateau.set(nEsclave, temperature);
                }
            }       
            plateau.printAll();

            ihm.FenetreGraphique_representer_plaque(plateau);
            ihm.FenetreGraphique_rendre();
        }

        // Envoi du message de fin de simulation au maitre
        printf ("Fils %d : Coordinateur : Envoi vers le pere !\n", myrank);
        MPI_Send(&endSignal, 1, MPI_CHAR, 0, 0, parent);

        //usleep(microseconds);*/
        ihm.FenetreGraphique_fermer();
    }
    printf ("Fils %d : Coordinateur : fin !\n", myrank);

    MPI_Finalize();

    return 0;
}
