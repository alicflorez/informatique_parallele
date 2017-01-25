#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include "Case_Plateau.h"

using namespace std;

int myrank;

void  PlateauToString(int nbLig, int nbCol, int **plateau){
	
    printf("Fils %d : Affichage des températures voisines : \n", myrank );
    for (int i = 0; i < nbLig; i++)
    {
        for(int j = 0; j < nbCol; j++){
            printf("| %d ", plateau[i][j] );
        }
        printf("|\n");
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

    // Propriétés
    double temperature;
    double temperatureAmbiante;
    int nbCol, nbLig, positionLig, positionCol, nbCycles;
    char endSignal = 'e';
    int **voisins;
    double **tempVoisins;
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
        MPI_Recv(&nbCol, 1, MPI_INT, 0, 0, parent, &etat);
        //printf("Fils %d : Esclave a reçu la longueur %d du plateau\n", myrank, nbCol );
        // Reception de la longueur de la part du maitre
        MPI_Recv(&nbLig, 1, MPI_INT, 0, 0, parent, &etat);
        //printf("Fils %d : Esclave a reçu la longueur %d du plateau\n", myrank, nbCol );

        MPI_Recv(&nbCycles, 1, MPI_INT, 0, 0, parent, &etat);
        
        // Reception de la température de la dalle
        MPI_Recv(&temperature, 1, MPI_DOUBLE, 0, 0, parent, &etat);
        //printf ("Fils %d : Esclave : La temperature est de %.3lf !\n", myrank, temperature);

        // Envoi du message de "bonne reception" au MAITRE
        MPI_Send(&endSignal, 1, MPI_CHAR, 0, 0, parent);
        //printf ("Fils %d : Esclave : Envoi vers le pere !\n", myrank);
        
        
        // Reception de la température ambiante de la part du coordinateur
        MPI_Recv(&temperatureAmbiante, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &etat);
        //printf ("Fils %d : Esclave : La temperature ambiante est de %.3lf !\n", myrank, temperatureAmbiante);
        
        MPI_Send(&temperature, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        
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

        for (int cycle=0; cycle < nbCycles; cycle++) {
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

            // Envoi du ACK au coordinateur
            MPI_Send(&temperature, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
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

    MPI_Finalize();
    return 0;
}