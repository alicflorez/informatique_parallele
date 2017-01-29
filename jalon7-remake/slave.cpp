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
            for(int j=0; j<tailleCoteCase+2; j++) {
            	tempVoisins[i][j]=0.0;
            }
        }

        int **macroVoisins = new int *[3];
        //On trouve les voisins
        for(int i=-1; i<=1; i++) {
            macroVoisins[i+1]=new int[3];

            for(int j=-1; j<=1; j++) {
                if (myrank<=nbCol && i==-1 || myrank>nbCol*(nbLig-1) && i==1 || myrank%nbCol==0 && j==1 || (myrank+j)%nbCol==0 && j==-1) {
                    macroVoisins[i+1][j+1]=-1;
                    tempVoisins[(i==1?i+tailleCoteCase:i+1)][(j==1?j+tailleCoteCase:j+1)]=temperatureAmbiante;
                    if (i==0) for(int k=2; k<=tailleCoteCase; k++) tempVoisins[k][(j==-1?0:tailleCoteCase+1)]=temperatureAmbiante;
                    if (j==0) for(int k=2; k<=tailleCoteCase; k++) tempVoisins[(i==-1?0:tailleCoteCase+1)][k]=temperatureAmbiante;
                } else {
                    macroVoisins[i+1][j+1]=myrank+i*nbCol+j;
                }
            }
        }

        for (int cycle=0; cycle < nbCycles; cycle++) {
            // Envoi de la température à tous les voisins
            for(int i=0; i<3; i++) {
                for(int j=0; j<3; j++) {
                    if (macroVoisins[i][j]!=-1 && macroVoisins[i][j]!=myrank) {
                        MPI_Isend(temperature, tailleCoteCase*tailleCoteCase, MPI_DOUBLE, macroVoisins[i][j], 0, MPI_COMM_WORLD, &req);
                        //MPI_Request_free(&req);
                    }
                }
            }
            //printf("Fils %d : Température envoyée à tous les voisins !\n", myrank );

            // Reception de la temperature de la part des voisins
            for(int i=0; i<3; i++) {
                for(int j=0; j<3; j++) {
                    if (macroVoisins[i][j] != myrank) {
                        if (macroVoisins[i][j] != -1 ) {
                            double temp[tailleCoteCase*tailleCoteCase];
                            MPI_Recv(temp, tailleCoteCase*tailleCoteCase, MPI_DOUBLE, macroVoisins[i][j], 0, MPI_COMM_WORLD, &etat);


                            if (i==1) for(int k=1; k<=tailleCoteCase; k++) tempVoisins[k][(j==0?0:tailleCoteCase+1)]=temp[(k-1)*tailleCoteCase+(j==0?tailleCoteCase-1:0)];
                            if (j==1) for(int k=1; k<=tailleCoteCase; k++) tempVoisins[(i==0?0:tailleCoteCase+1)][k]=temp[(i==0?tailleCoteCase-1:0)*tailleCoteCase+(k-1)];
                            if (i==0 && j==0 || i==0 && j==2 || i==2 && j==0 || i==2 && j==2) {
                            	tempVoisins[(i==0?0:tailleCoteCase+1)][(j==0?0:tailleCoteCase+1)]=temp[(tailleCoteCase-1-i)*tailleCoteCase+(tailleCoteCase-1-j)];
                            }

                        }
                    } else {
                        for(int y=1; y<=tailleCoteCase; y++)
                            for(int x=1; x<=tailleCoteCase; x++)
                                tempVoisins[y][x]=temperature[(y-1)*tailleCoteCase+(x-1)];
                    }
                }
            }

//        for (int i = 0; i < tailleCoteCase+2; i++) {
//          cout << myrank << "):  ";
//            for(int j = 0; j < tailleCoteCase+2; j++){
//                printf("| %.3lf ", tempVoisins[i][j] );
//            }
//            printf("|\n");
//        }
//		printf("\n");


			// Calcul de la nouvelle température ( moyenne de toutes les température des voisins + le carré)
			for(int y=1; y<tailleCoteCase+1; y++) {
				for(int x=1; x<tailleCoteCase+1; x++) {
//					cout<<myrank<<") "<<y<<":"<<x;
					double sum=0.0;
					for(int i=-1; i<=1; i++) {
						for(int j=-1; j<=1; j++) {
							sum+=tempVoisins[y+i][x+j];
//							cout<<" ("<<i<<":"<<j<<") "<<tempVoisins[y+i][x+j];
						}
					}
					temperature[(y-1)*tailleCoteCase+(x-1)] = sum/(tailleCoteCase*tailleCoteCase);
//					cout<<" = "<<(y-1)*tailleCoteCase+(x-1)<<") "<<sum/(tailleCoteCase*tailleCoteCase)<<endl;
				}
			}

            // Envoi du ACK au coordinateur
            MPI_Send(temperature, tailleCoteCase*tailleCoteCase, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }

        // Libération de l'espace mémoire
        for(int i = 0; i < 3; i++){
            delete[] macroVoisins[i];
        }
        delete[] macroVoisins;

        for(int i=0; i<tailleCoteCase+2; i++){
			delete[] tempVoisins[i];
        }
        delete[] tempVoisins;

    }

    printf ("Fils %d : Slave: Fin !\n", myrank);

    MPI_Finalize();
    return 0;
}
