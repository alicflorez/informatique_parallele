/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   plaque_metal.h
 * Author: johnlab
 *
 * Created on January 19, 2017, 12:04 PM
 */

#ifndef PLAQUE_METAL_H
#define PLAQUE_METAL_H

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "Case_Plateau.h"

using namespace std;

class Plaque_Metal {
private: 
    //Case_Plateau 
    double **plateau; int plateauNbLignes, plateauNbColonnes, tailleMatriceCase;

    void setPlateauNbLignes(int y);
    void setPlateauNbColonnes(int x);
    void setTailleMatriceCase(int m);
public: 
    // Constructeurs
    Plaque_Metal(char *filename, int tailleMatriceCase);

    double getAverage(int y, int x);
    double getAverageByRank(int caseRank);
    double *getCaseByRank(int caseRank);
    void set(int y, int x, double val);

    int getPlateauNbLignes();
    int getPlateauNbColonnes();
    int getTailleMatriceCase();
    
    void print();
};
#endif /* PLAQUE_METAL_H */
