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
//        printf ("Fils %d : Esclave : La temperature est de %.3lf !\n", myrank, temperature[]);

        
        // Envoi du message de "bonne reception" au MAITRE
        MPI_Send(&endSignal, 1, MPI_CHAR, 0, 0, parent);
        //printf ("Fils %d : Esclave : Envoi vers le pere !\n", myrank);
        
        
        // Reception de la température ambiante de la part du coordinateur
        MPI_Recv(&temperatureAmbiante, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &etat);
        //printf ("Fils %d : Esclave : La temperature ambiante est de %.3lf !\n", myrank, temperatureAmbiante);
        
        MPI_Send(temperature, tailleCoteCase*tailleCoteCase, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        
        double **tempVoisins=new double *[tailleCoteCase+2];
        for(int i=0; i<tailleCoteCase+2; i++) { 
            tempVoisins[i]=new double[tailleCoteCase+2];
        }
        
        double **OLDtempVoisins = new double *[3];
        int **macroVoisins = new int *[3];   
        //On trouve les voisins
        for(int i=-1; i<=1; i++) { 
            macroVoisins[i+1]=new int[3];
            OLDtempVoisins[i+1]=new double[3];
            
            for(int j=-1; j<=1; j++) {
                if (myrank<=nbCol && i==-1 || myrank>nbCol*(nbLig-1) && i==1 || myrank%nbCol==0 && j==1 || (myrank+j)%nbCol==0 && j==-1) {
                    macroVoisins[i+1][j+1]=-1;
                    OLDtempVoisins[i+1][j+1]=temperatureAmbiante;
                    tempVoisins[(i==1?i+tailleCoteCase:i+1)][(j==1?j+tailleCoteCase:j+1)]=temperatureAmbiante;
                    if (i==0) for(int k=2; k<=tailleCoteCase; k++) tempVoisins[k][j+1]=temperatureAmbiante;
                    if (j==0) for(int k=2; k<=tailleCoteCase; k++) tempVoisins[i+1][k]=temperatureAmbiante;
                } else {
                    macroVoisins[i+1][j+1]=myrank+i*nbCol+j;
                }
            }
        }
                 
        for (int cycle=0; cycle < nbCycles; cycle++) {
            // Envoi de la température à tous les voisins
            for(int i = 0; i < 3; i++) {
                for(int j = 0; j < 3; j++) {
                    if (macroVoisins[i][j] != -1 && macroVoisins[i][j] != myrank) {
                        MPI_Isend(temperature, tailleCoteCase*tailleCoteCase, MPI_DOUBLE, macroVoisins[i][j], 0, MPI_COMM_WORLD, &req);
                        
                        //MPI_Request_free(&req);
                        //printf("Fils %d : Envoi de la temperature au voisins Fils %d \n", myrank, voisins[i][j]);
                    }
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
                            
                            if (i==1) for(int k=1; k<=tailleCoteCase; k++) tempVoisins[k][(j==0?j:tailleCoteCase+1)]=temp[k-1];
                            if (j==1) for(int k=1; k<=tailleCoteCase; k++) tempVoisins[(i==0?i:tailleCoteCase+1)][k]=temp[k-1];
//                            for (int a=0; a<3; a++) {
//                                cout << myrank << ") voisin: ";
//                                for(int b=0; b<3; b++){
//                                    printf("| %.3lf ", temp[a*tailleCoteCase+b] );
//                                }
//                                printf("|\n");
//                            }
                            
                            OLDtempVoisins[i][j] = temp[0];
                            //printf("Fils %d : Température reçu de la part de Slave %d ! \n", myrank, voisins[i][j] );
                        }			
                    } else {
                        for(int y=1; y<=tailleCoteCase; y++)
                            for(int x=1; x<=tailleCoteCase; x++)
                                tempVoisins[y][x]=temperature[0];
                        OLDtempVoisins[1][1] = temperature[0];		
                    }			
                }
            }
          
        for (int i = 0; i < tailleCoteCase+2; i++) {
          cout << myrank << "):  ";   
            for(int j = 0; j < tailleCoteCase+2; j++){
                printf("| %.3lf ", tempVoisins[i][j] );
            }
            printf("|\n");
        }
            printf("\n");      
            // Calcul de la nouvelle température ( moyenne de toutes les température des voisins + le carré)
            double sum=0;
            for(int i = 0; i < 3; i++) 
                for(int j = 0; j < 3; j++)
                    sum += OLDtempVoisins[i][j];

            for(int i=0; i<tailleCoteCase*tailleCoteCase; i++)
                temperature[i] = sum/9;

//            printf("Fils %d : L'Esclave a mis à jour sa temperature ! \n", myrank );
            //PlateauDoubleToString(3,3, OLDtempVoisins);

            // Envoi du ACK au coordinateur
            MPI_Send(temperature, tailleCoteCase*tailleCoteCase, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }

        // Libération de l'espace mémoire
        for(int i = 0; i < 3; i++){
            delete[] OLDtempVoisins[i];
            delete[] macroVoisins[i];
        }
        delete[] OLDtempVoisins;
        delete[] macroVoisins;

    }

    printf ("Fils %d : Slave: Fin !\n", myrank);

    MPI_Finalize();
    return 0;
}