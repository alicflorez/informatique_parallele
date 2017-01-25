

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "Plaque_Metal.h"

// Constructeurs      
Plaque_Metal::Plaque_Metal(char *filename, int tailleMatriceCase) {
    string line;
    string file_text="";
    string number;
    setTailleMatriceCase(tailleMatriceCase);
    
    ifstream file; file.open(filename); 
    int plateauNbLignes=0;
    while(getline(file, line)) {
        int plateauNbColonnes=0;
        stringstream ssin(line);
        while (ssin.good()){
            ssin >> number;
            if (plateauNbLignes!=0||plateauNbColonnes!=0) file_text+=" ";
            file_text+=number.c_str();
            
            plateauNbColonnes++;
            if (plateauNbLignes==0) setPlateauNbColonnes(plateauNbColonnes);
        }
        cout << endl;

        plateauNbLignes++;
    }
    setPlateauNbLignes(plateauNbLignes);
    file.close();
    
    this->plateau=new double*[getPlateauNbLignes()*getPlateauNbColonnes()];
    stringstream ssin(file_text);
    
    for (int y=0; y<getPlateauNbLignes(); y++) {
        for (int x=0; x<getPlateauNbColonnes(); x++) {
            ssin >> number;
            double *casePlateau = new double[getTailleMatriceCase()*getTailleMatriceCase()];
            for (int cursor=0; cursor<getTailleMatriceCase()*getTailleMatriceCase(); cursor++) {
                casePlateau[cursor]=atof(number.c_str());
            }
            plateau[y*getPlateauNbColonnes()+x]=casePlateau;
        }
    }
    
    print();
}

double Plaque_Metal::getAverage(int y, int x) {
    double total=0;
    for (int i=0; i<getTailleMatriceCase()*getTailleMatriceCase(); i++) {
        total+=plateau[y*getPlateauNbColonnes()+x][i];
    }
    return total/(getTailleMatriceCase()*getTailleMatriceCase());
}
double Plaque_Metal::getAverageByRank(int caseRank) {
    double total=0;
    for (int i=0; i<getPlateauNbLignes(); i++) {
        total+=plateau[caseRank][i];
    }
    return total/(getTailleMatriceCase()*getTailleMatriceCase());
}
double *Plaque_Metal::getCaseByRank(int caseRank) {return plateau[caseRank];}

void Plaque_Metal::set(int y, int x, double val) {
    for (int i=0; i<getPlateauNbLignes(); i++) {
        for (int j=0; j<getPlateauNbColonnes(); j++) {
            plateau[y*getPlateauNbColonnes()+x][i*getTailleMatriceCase()+j]=val;
        }
    }
}

int Plaque_Metal::getPlateauNbLignes() {return plateauNbLignes;}
int Plaque_Metal::getPlateauNbColonnes() {return plateauNbColonnes;}
int Plaque_Metal::getTailleMatriceCase() {return tailleMatriceCase;}

void Plaque_Metal::setPlateauNbLignes(int y) {this->plateauNbLignes=y;}
void Plaque_Metal::setPlateauNbColonnes(int x) {this->plateauNbColonnes=x;}
void Plaque_Metal::setTailleMatriceCase(int m) {this->tailleMatriceCase=m;}

void Plaque_Metal::print(){
    for (int y=0; y<getPlateauNbLignes(); y++) {
        for(int x=0; x<getPlateauNbColonnes(); x++) {
            getAverage(y, x);
            printf("| %.3lf ", getAverage(y, x));
        }
        printf("|\n");
    }
}
    