/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   drawing.h
 * Author: johnlab
 *
 * Created on January 24, 2017, 8:50 AM
 */

#ifndef IHM_H
#define IHM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <GL/glut.h>
#include "Plaque_Metal.h"

class IHM {
private:
    double xSize;
    double ySize;
    SDL_Renderer* displayRenderer;
public:
    IHM(int largeurDeLaFenetre, int hauteurDeLaFenetre, 
        double xMin, double xMax, double yMin, double yMax); 
    SDL_Renderer* FenetreGraphique_creer(int largeurDeLaFenetre, int hauteurDeLaFenetre, 
        double xMin, double xMax, double yMin, double yMax);
    void FenetreGraphique_initialiser();
    void FenetreGraphique_rendre();
    void FenetreGraphique_fermer();

    void FenetreGrahique_dessinerRectangle(double x1, double y1, double x2, double y2,
        float fondR, float fondV, float fondB,
        float formeR, float formeV, float formeB);
    void FenetreGraphique_representer_plaque(double **plateau, int nbLig, int nbCol);
    void FenetreGraphique_representer_plaque(Plaque_Metal plateau);
};

#endif /* IHM_H */

