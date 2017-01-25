/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "IHM.h"

IHM::IHM(int largeurDeLaFenetre, int hauteurDeLaFenetre, double xMin, double xMax, double yMin, double yMax) {
    xSize=1.6;
    ySize=1.6;
    displayRenderer=FenetreGraphique_creer(largeurDeLaFenetre, hauteurDeLaFenetre, xMin, xMax, yMin, yMax);
}

SDL_Renderer* IHM::FenetreGraphique_creer (int largeurDeLaFenetre, int hauteurDeLaFenetre, double xMin, 
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
void IHM::FenetreGraphique_initialiser () {
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    //glOrtho (-3, 3, -3, 3, -1.0, 1.0);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}
void IHM::FenetreGraphique_rendre () {
    SDL_RenderPresent (displayRenderer);
    SDL_Delay(800);
}
void IHM::FenetreGraphique_fermer () {
    SDL_Quit();
}
void IHM::FenetreGrahique_dessinerRectangle (double x1, double y1, double x2, double y2,
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
void IHM::FenetreGraphique_representer_plaque (double **plateau, int nbLig, int nbCol) {
    for (int i=0; i < nbLig; i++) {	
        for (int j=0; j < nbCol; j++) {	
            FenetreGrahique_dessinerRectangle (
                    -xSize/2+(xSize/nbCol)*j, 
                    -(-ySize/2+(ySize/nbLig)*(i+1)), 
                    -xSize/2+(xSize/nbCol)*(j+1), 
                    -(-ySize/2+(ySize/nbLig)*i), 
                    plateau[i][j]/90, 0, 1-plateau[i][j]/90, 
                    0.5, 0.5, 0.5);
        }
    }
}