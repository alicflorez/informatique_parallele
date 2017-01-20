

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "Plaque_Metal.h"
// Constructeurs      
Plaque_Metal::Plaque_Metal(char *filename) {
    ifstream file;
    file.open(filename); 
    string line;
    string file_text="";
    string number;
    
    int y=0;
    while(getline(file, line)) {
        int x=0;
        stringstream ssin(line);
        while (ssin.good()){
            ssin >> number;
            if (y!=0||x!=0) file_text+=" ";
            file_text+=number.c_str();
            
            x++;
            if (y==0) this->x=x;
        }
        cout << endl;

        y++;
    }
    this->y=y;
    file.close();
    
    this->plateau=new Case_Plateau[getY()*getX()];
    stringstream ssin(file_text);
    
    for (int y=0; y<getY(); y++) {
        for (int x=0; x<getX(); x++) {
            ssin >> number;
            plateau[y*getX()+x]=Case_Plateau(atof(number.c_str()), 3);
        }
    }
    
    print();
}

double Plaque_Metal::get(int y, int x) {return plateau[y*getX()+x].getAverage();}

void Plaque_Metal::set(int y, int x, double val) {plateau[y*getX()+x].set(val);}

int Plaque_Metal::getY() {return y;}
int Plaque_Metal::getX() {return x;}

void Plaque_Metal::print(){
    for (int y=0; y<getY(); y++) {
        for(int x=0; x<getX(); x++) {
            printf("| %.3lf ", get(y, x));
        }
        printf("|\n");
    }
}
    