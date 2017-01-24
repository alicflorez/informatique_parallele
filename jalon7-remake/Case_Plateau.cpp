/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Case_Plateau.h"    

Case_Plateau::Case_Plateau() {}
Case_Plateau::Case_Plateau(double temperature, int taille_matrice) {
    this->matrice_case=new double[taille_matrice*taille_matrice];
    this->taille_matrice=taille_matrice;
    for (int i=0; i<(taille_matrice*taille_matrice); i++) {
        this->matrice_case[i]=temperature;
    }
}   

void Case_Plateau::set(double temperature) {
    for (int i=0; i<this->taille_matrice*this->taille_matrice; i++) {
        this->matrice_case[i]=temperature;
    }
}
double *Case_Plateau::get() {
    return matrice_case;
}
double Case_Plateau::getAverage() {
    int total=0;
    for (int i=0; i<this->taille_matrice*this->taille_matrice; i++) {
        total+=this->matrice_case[i];
    }
    return total/(this->taille_matrice*this->taille_matrice);
}
int Case_Plateau::getTaille() {
    return taille_matrice;
}
