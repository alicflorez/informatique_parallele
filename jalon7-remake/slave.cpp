#include <mpi.h>
#include <stdio.h>
#include <iostream>

using namespace std;

int myrank;

int main( int argc, char *argv[] ) {

    // Propriétés
    double temperatureAmbiante;
    int nbCol, nbLig, tailleCoteCase, positionLig, positionCol, nbCycles;
    char endSignal = 'e';
    int **macroVoisins;
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
        
        // Reception de la longueur de la part du maitre
        MPI_Recv(&tailleCoteCase, 1, MPI_INT, 0, 0, parent, &etat);
        
        MPI_Recv(&nbCycles, 1, MPI_INT, 0, 0, parent, &etat);
        
        double temperature[tailleCoteCase*tailleCoteCase];
        // Reception de la température de la dalle
        MPI_Recv(temperature, tailleCoteCase*tailleCoteCase, MPI_DOUBLE, 0, 0, parent, &etat);
//        double temperature;
//        MPI_Recv(&temperature, 1, MPI_DOUBLE, 0, 0, parent, &etat);
//        printf ("Fils %d : Esclave : La temperature est de %.3lf !\n", myrank, temperature[]);

        
        // Envoi du message de "bonne reception" au MAITRE
        MPI_Send(&endSignal, 1, MPI_CHAR, 0, 0, parent);
        //printf ("Fils %d : Esclave : Envoi vers le pere !\n", myrank);
        
        
        // Reception de la température ambiante de la part du coordinateur
        MPI_Recv(&temperatureAmbiante, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &etat);
        //printf ("Fils %d : Esclave : La temperature ambiante est de %.3lf !\n", myrank, temperatureAmbiante);
        
//        MPI_Send(&temperature, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        MPI_Send(temperature, tailleCoteCase*tailleCoteCase, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        
        
        //Calcul de la position du carré sur le plateau
        positionCol = (myrank-1)%nbCol;
        positionLig = (myrank-1)/nbCol;

        tempVoisins = new double *[3];
        macroVoisins = new int *[3];
        for(int i=0; i<3; i++){
            macroVoisins[i]=new int[3];
            tempVoisins[i]=new double [3];
        }
        
        
        
        //On trouve les voisins
        for(int i=-1; i<=1; i++)
            for(int j=-1; j<=1; j++) {
                if (myrank<=nbCol && i==-1 || myrank>nbCol*(nbLig-1) && i==1 || myrank%nbCol==0 && j==1 || (myrank+j)%nbCol==0 && j==-1) {
                    macroVoisins[i+1][j+1]=-1;
                } else {
                    macroVoisins[i+1][j+1]=myrank+i*nbCol+j;
                }
            }
     
//    cout << myrank << ")"  << endl;   
//    for (int i = 0; i < 3; i++) {
//        for(int j = 0; j < 3; j++){
//            printf("| %d ", macroVoisins[i][j] );
//        }
//        printf("|\n");
//    }
            
        for (int cycle=0; cycle < nbCycles; cycle++) {
            // Envoi de la température à tous les voisins
            for(int i = 0; i < 3; i++)
                for(int j = 0; j < 3; j++) {
                    if (macroVoisins[i][j] != -1 && macroVoisins[i][j] != myrank) {
//                        MPI_Isend(&temperature, 1, MPI_DOUBLE, macroVoisins[i][j], 0, MPI_COMM_WORLD, &req);
                        MPI_Isend(temperature, tailleCoteCase*tailleCoteCase, MPI_DOUBLE, macroVoisins[i][j], 0, MPI_COMM_WORLD, &req);
                        
                        //MPI_Request_free(&req);
                        //printf("Fils %d : Envoi de la temperature au voisins Fils %d \n", myrank, voisins[i][j]);
                    }
                }
            //printf("Fils %d : Température envoyée à tous les voisins !\n", myrank );

            // Reception de la temperature de la part des voisins
            for(int i = 0; i < 3; i++) {
                for(int j = 0; j < 3; j++) {
                    if (macroVoisins[i][j] != myrank) {
                        if (macroVoisins[i][j] != -1 ) {
                            double temp[tailleCoteCase*tailleCoteCase];
                            MPI_Recv(temp, tailleCoteCase*tailleCoteCase, MPI_DOUBLE, macroVoisins[i][j], 0, MPI_COMM_WORLD, &etat);
//                
//            cout << myrank << ")"  << endl;   
//            for (int a=0; a<3; a++) {
//                for(int b=0; b<3; b++){
//                    printf("| %.3lf ", temp[a*tailleCoteCase+b] );
//                }
//                printf("|\n");
//            }
                            tempVoisins[i][j] = temp[0];
                            //printf("Fils %d : Température reçu de la part de Slave %d ! \n", myrank, voisins[i][j] );
                        } else 
                            tempVoisins[i][j] = temperatureAmbiante;						
                    } else
                        tempVoisins[i][j] = temperature[0];					
                }
            }
            // Calcul de la nouvelle température ( moyenne de toutes les température des voisins + le carré)
            double sum=0;
            for(int i = 0; i < 3; i++) 
                for(int j = 0; j < 3; j++)
                    sum += tempVoisins[i][j];

            for(int i=0; i<tailleCoteCase*tailleCoteCase; i++)
                temperature[i] = sum/tailleCoteCase*tailleCoteCase ;

//            printf("Fils %d : L'Esclave a mis à jour sa temperature ! \n", myrank );
            //PlateauDoubleToString(3,3, tempVoisins);

            // Envoi du ACK au coordinateur
            MPI_Send(temperature, tailleCoteCase*tailleCoteCase, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }

        // Libération de l'espace mémoire
        for(int i = 0; i < 3; i++){
            delete[] macroVoisins[i];
            delete[] tempVoisins[i];
        }
    

        delete[] tempVoisins;
        delete[] macroVoisins;

        for(int i = 0; i < nbLig; i++)
        delete[] rangPlateau[i];

        delete[] rangPlateau;
    }

    printf ("Fils %d : Slave: Fin !\n", myrank);

    MPI_Finalize();
    return 0;
}