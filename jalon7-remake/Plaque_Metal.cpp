

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "Plaque_Metal.h"

// Constructeurs      
Plaque_Metal::Plaque_Metal(char *filename, int tailleCoteCase) {
    string line;
    string file_text="";
    string number;
    setTailleCoteCase(tailleCoteCase);
    
    ifstream file; file.open(filename); 
    int nbLignes=0;
    while(getline(file, line)) {
        int nbColonnes=0;
        stringstream ssin(line);
        while (ssin.good()){
            ssin >> number;
            if (nbLignes!=0||nbColonnes!=0) file_text+=" ";
            file_text+=number.c_str();
            
            nbColonnes++;
            if (nbLignes==0) setNbColonnes(nbColonnes);
        }
        cout << endl;

        nbLignes++;
    }
    setNbLignes(nbLignes);
    file.close();
    
    plateau=new double*[getNbLignes()*getNbColonnes()];
    stringstream ssin(file_text);
    
    for (int casePleateau=0; casePleateau<getNbLignes()*getNbColonnes(); casePleateau++) {
        ssin >> number;
        double *casePlateau = new double[getTailleCoteCase()*getTailleCoteCase()];
        for (int cursor=0; cursor<getTailleCoteCase()*getTailleCoteCase(); cursor++) {
            casePlateau[cursor]=atof(number.c_str());
        }
        plateau[casePleateau]=casePlateau;
    }

    findMaxTemperature();
    printAll();
}

Plaque_Metal::Plaque_Metal(int nbLig, int nbCol, int tailleCoteCase) {
    setNbColonnes(nbCol);
    setNbLignes(nbLig);
    setTailleCoteCase(tailleCoteCase);
    plateau=new double*[nbLig*nbCol];
    for (int casePleateau=0; casePleateau<nbLig*nbCol; casePleateau++) {
        plateau[casePleateau]=new double[tailleCoteCase*tailleCoteCase];
        for (int cursor=0; cursor<tailleCoteCase*tailleCoteCase; cursor++) {
            plateau[casePleateau][cursor]=0.0;
        }
    }
}

void Plaque_Metal::findMaxTemperature() {    
    double max=0.0;
    for (int i=0; i<getNbLignes()*getNbColonnes(); i++) {
        for (int j=0; j<getTailleCoteCase()*getTailleCoteCase(); j++) {
            if (plateau[i][j]>max) max=plateau[i][j];
        }
    }
    maxTemperature=max;
}
double Plaque_Metal::getMaxTemperature() {
    return maxTemperature;
}

double Plaque_Metal::getAverage(int y, int x) {
    double total=0;
    for (int i=0; i<getTailleCoteCase()*getTailleCoteCase(); i++) {
        total+=plateau[y*getNbColonnes()+x][i];
    }
    return total/(getTailleCoteCase()*getTailleCoteCase());
}
double Plaque_Metal::getAverageByRank(int caseRank) {
    double total=0;
    for (int i=0; i<getTailleCoteCase()*getTailleCoteCase(); i++) {
        total+=plateau[caseRank][i];
    }
    return total/(getTailleCoteCase()*getTailleCoteCase());
}
double *Plaque_Metal::getCaseByRank(int caseRank) {return plateau[caseRank];}

void Plaque_Metal::set(int y, int x, double val) {
    for (int i=0; i<getNbLignes(); i++) {
        for (int j=0; j<getNbColonnes(); j++) {
            plateau[y*getNbColonnes()+x][i*getTailleCoteCase()+j]=val;
        }
    }
}
void Plaque_Metal::set(int caseRank, double *val) {
    for (int i=0; i<getTailleCoteCase()*getTailleCoteCase(); i++) {
        plateau[caseRank-1][i]=val[i];
    }
}

int Plaque_Metal::getNbLignes() {return nbLignes;}
int Plaque_Metal::getNbColonnes() {return nbColonnes;}
int Plaque_Metal::getTailleCoteCase() {return tailleCoteCase;}

void Plaque_Metal::setNbLignes(int y) {this->nbLignes=y;}
void Plaque_Metal::setNbColonnes(int x) {this->nbColonnes=x;}
void Plaque_Metal::setTailleCoteCase(int m) {this->tailleCoteCase=m;}

void Plaque_Metal::printAll() {
    for (int X=0; X<getNbColonnes()*getTailleCoteCase(); X++) {
        printf("--------");
    }
    printf("-------\n");
    for (int Y=0; Y<getNbLignes(); Y++) {
        for (int y=0; y<getTailleCoteCase(); y++) {
            for (int X=0; X<getNbColonnes()*getTailleCoteCase(); X++) {
                if (X%getTailleCoteCase()==0)printf("|");
                printf(" %.3lf ", getCaseByRank(Y*getNbColonnes()+int(X/getTailleCoteCase()))[y*getTailleCoteCase()+X%getTailleCoteCase()]);
            }
            printf("|\n");
        }
        for (int X=0; X<getNbColonnes()*getTailleCoteCase(); X++) {
            printf("--------");
        }
        printf("-------\n");
    }
}
void Plaque_Metal::print() {
    for (int i = 0; i < getNbLignes(); i++) {
        for(int j = 0; j < getNbColonnes(); j++){
            printf("| %.3lf ", getAverage(i, j) );
        }
        printf("|\n");
    }
}
