#include "jumpif.h"
using namespace std;

Jumpif::Jumpif(int c) : c(c){}

void Jumpif::ejecutar(Programa* programa){
    int h = programa->getPila().top();
    programa->getPila().pop();
    if (h >= 0){
        programa->salto(c);
    }else{
        programa->inc();
    }
}

string Jumpif::mostrar(){
    return("jumpif " + to_string(c));
}
