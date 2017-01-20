/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Case.h
 * Author: johnlab
 *
 * Created on January 20, 2017, 5:46 AM
 */

#ifndef CASE_PLATEAU_H
#define CASE_PLATEAU_H

#include <stdlib.h>
#include <iostream>
using namespace std;

class Case_Plateau {
private:
    double *matrice_case;
    int taille_matrice;
public:
    Case_Plateau();
    Case_Plateau(double temperature, int taille_matrice);
    
    void set(double temperature);
    double *get();
    double getAverage();
};

#endif /* CASE_PLATEAU_H */

