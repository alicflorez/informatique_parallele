#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glu.h>

unsigned int microseconds = 3000;

using namespace std;



SDL_Renderer* FenetreGraphique_creer (int largeurDeLaFenetre, int hauteurDeLaFenetre, double xMin, 
										double xMax, double yMin, double yMax) {
    SDL_Init (SDL_INIT_VIDEO);
    SDL_Window* displayWindow;
    SDL_RendererInfo displayRendererInfo;
    SDL_Renderer* displayRenderer;
    SDL_CreateWindowAndRenderer (largeurDeLaFenetre, hauteurDeLaFenetre, SDL_WINDOW_OPENGL,
    &displayWindow, &displayRenderer);
    SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);
    glViewport (0, 0, (GLsizei) largeurDeLaFenetre, (GLsizei) hauteurDeLaFenetre);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    glOrtho (xMin, xMax, yMin, yMax, 1, -1);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );
    return displayRenderer;
} 

void FenetreGraphique_initialiser () {
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    //glOrtho (-3, 3, -3, 3, -1.0, 1.0);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}

void FenetreGraphique_rendre (SDL_Renderer* displayRenderer) {
    SDL_RenderPresent (displayRenderer);
    SDL_Delay(800);
}

void FenetreGraphique_fermer () {
    SDL_Quit();
}

void FenetreGrahique_dessinerRectangle (double x1, double y1, double x2, double y2,
										float fondR, float fondV, float fondB,
										float formeR, float formeV, float formeB) {
    glColor3f (fondR, fondV ,fondB);

    glBegin (GL_QUADS);
        glVertex2f (x1, y1);
        glVertex2f (x2, y1);
        glVertex2f (x2, y2);
        glVertex2f (x1, y2);
    glEnd ();

    glColor3f (formeR, formeV ,formeB);

    glBegin (GL_LINE_LOOP);
        glVertex2f (x1, y1);
        glVertex2f (x2, y1);
        glVertex2f (x2, y2);
        glVertex2f (x1, y2);
    glEnd ();
}

double xSize=1.6;
double ySize=1.6;
void FenetreGraphique_representer_plaque (double **plateau, int nbLig, int nbCol) {
    for (int i=0; i < nbLig; i++) {	
        for (int j=0; j < nbCol; j++) {	
            FenetreGrahique_dessinerRectangle (-xSize/2+(xSize/nbCol)*j, -ySize/2+(ySize/nbLig)*(i+1), -xSize/2+(xSize/nbCol)*(j+1), -ySize/2+(ySize/nbLig)*i, 
                    plateau[i][j]/75, 0, 1-plateau[i][j]/75, 
                    0.5, 0.5, 0.5);
        }
    }
}

/*
int main(int argc, char *argv[]) {
	SDL_Renderer* displayRenderer = FenetreGraphique_creer (800, 600, -2, 2, -2, 2);
	FenetreGraphique_initialiser ();
	FenetreGrahique_dessinerRectangle (-0.8, -0.8, 0.8, 0.8, 1, 0, 1, 0, 1, 0);
	FenetreGraphique_rendre (displayRenderer);
	FenetreGraphique_fermer ();
	return 0;
} 
*/

int main( int argc, char *argv[] ) {
    // Propriétés
    double temperatureAmbiante;
    char endSignal = 'c';
    int myrank, N, nbLig, nbCol, nbCycles;
    double **plateau;

    // Initialisation communication
    MPI_Comm parent;
    MPI_Status etat;
    MPI_Init (&argc, &argv);
    MPI_Comm_get_parent (&parent);
    MPI_Comm_rank (MPI_COMM_WORLD,&myrank);

    // IHM 

    SDL_Renderer* displayRenderer = FenetreGraphique_creer (800, 600, -2, 2, -2, 2);

    if (parent == MPI_COMM_NULL) {
        printf ("Fils %d : Coordinateur : Pas de pere !\n", myrank);
    } else {
        // Reception de la largeur du plateau
        MPI_Recv(&nbCol, 1, MPI_INT, 0, 0, parent, &etat);
        // Reception de la longueur du plateau
        MPI_Recv(&nbLig, 1, MPI_INT, 0, 0, parent, &etat);
        //printf("Le Coordinateur a reçu la longueur %d du plateau\n", nbLig);

        MPI_Recv(&nbCycles, 1, MPI_INT, 0, 0, parent, &etat);

        //FenetreGrahique_dessinerRectangle (-(0.95/(nbCol)), -(0.95/(nbLig)), (0.95/(nbCol/2)), (0.95/(nbLig/2)), 1, 0, 1, 0, 1, 0);
        // Calcul du nombre d'esclave
        N = nbCol*nbLig;

        // Reception de la temperature ambiante
        MPI_Recv(&temperatureAmbiante, 1, MPI_DOUBLE, 0, 0, parent, &etat);
        //printf ("Fils %d : Coordinateur : La temperature ambiante recue est de %.3lf !\n", myrank, temperatureAmbiante);


        plateau = new double *[nbLig];
        for(int i = 0; i < nbLig; i++)
            plateau[i] = new double[nbCol];


        FenetreGraphique_rendre (displayRenderer);
        FenetreGraphique_initialiser ();

        for (int iteration=0; iteration < nbCycles; iteration++){
            // Envoi de la temperature ambiante aux esclaves 10 fois
            for (int i=0; i < N; i++) {		
                //printf ("Coordinateur envoie temp ambiante vers %d\n", (i+1));
                MPI_Send(&temperatureAmbiante, 1, MPI_DOUBLE, i+1, 0, MPI_COMM_WORLD);
            }

            // Pour chaques esclaves 
            //Attente de la reception de la température de tous les esclaves
            int nEsclave = 0;

            for (int i=0; i < nbLig; i++) {	
                for (int j=0; j < nbCol; j++) {	
                    nEsclave++;
                    double temperature;					
                    //printf ("Coordinateur recoit ack de %d\n", nEsclave;
                    MPI_Recv(&temperature, 1, MPI_DOUBLE, nEsclave, 0, MPI_COMM_WORLD, &etat);
                    printf ("Coordinateur : La temperature de %d est de %.3lf !\n", nEsclave, temperature);

                    plateau[i][j] = temperature;
                }
            }
            //PlateauToString(nbLig,nbCol,plateau);

            FenetreGraphique_representer_plaque (plateau, nbLig, nbCol);
                    
            FenetreGraphique_rendre (displayRenderer);
        }

        // Envoi du message de fin de simulation au maitre
        printf ("Fils %d : Coordinateur : Envoi vers le pere !\n", myrank);
        MPI_Send(&endSignal, 1, MPI_CHAR, 0, 0, parent);


        for(int i = 0; i < nbLig; i++)
            delete[] plateau[i];

        delete[] plateau;

        //usleep(microseconds);*/
    }
    printf ("Fils %d : Coordinateur : fin !\n", myrank);

    MPI_Finalize();

    FenetreGraphique_fermer ();
    return 0;
}