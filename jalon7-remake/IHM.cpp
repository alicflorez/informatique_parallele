/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "IHM.h"

IHM::IHM(int largeurDeLaFenetre, int hauteurDeLaFenetre, double xMin, double xMax, double yMin, double yMax, int vitesse) {
    xSize=1.6;
    ySize=1.6;
    this->vitesse=vitesse;
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
    if (vitesse==0)
        SDL_Delay(1000);
    else if (vitesse==1)
        SDL_Delay(700);
    else if (vitesse==2)
        SDL_Delay(400);
    else
        SDL_Delay(100);
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
void IHM::FenetreGraphique_representer_plaque (Plaque_Metal plateau) {
    for (int Y=0; Y<plateau.getNbLignes(); Y++) {	
        for (int X=0; X<plateau.getNbColonnes(); X++) {	

            for (int y=0; y<plateau.getTailleCoteCase(); y++) {   
                for (int x=0; x<plateau.getTailleCoteCase(); x++) { 
                    FenetreGrahique_dessinerRectangle (
                            -xSize/2+(xSize/plateau.getNbColonnes())*X+(xSize/plateau.getNbColonnes()/plateau.getTailleCoteCase())*x+(x==0?0.05/plateau.getNbColonnes():0.0), 
                            -(-ySize/2+(ySize/plateau.getNbLignes())*Y+(ySize/plateau.getNbLignes()/plateau.getTailleCoteCase())*(y+1)), 
                            -xSize/2+(xSize/plateau.getNbColonnes())*X+(xSize/plateau.getNbColonnes()/plateau.getTailleCoteCase())*(x+1), 
                            -(-ySize/2+(ySize/plateau.getNbLignes())*Y+(ySize/plateau.getNbLignes()/plateau.getTailleCoteCase())*(y))-(y==0?0.05/plateau.getNbLignes():0.0), 
                            (plateau.getMaxTemperature()==0?0.0:plateau.getCaseByRank(Y*plateau.getNbColonnes()+X)[y*plateau.getTailleCoteCase()+x]/plateau.getMaxTemperature()), 0, (plateau.getMaxTemperature()==0?1.0:1-plateau.getCaseByRank(Y*plateau.getNbColonnes()+X)[y*plateau.getTailleCoteCase()+x]/plateau.getMaxTemperature()), 
                            0.5, 0.5, 0.5);
                }
            }
        }
    }
}