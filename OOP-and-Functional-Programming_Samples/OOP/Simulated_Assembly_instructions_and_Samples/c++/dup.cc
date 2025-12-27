#include "dup.h"
using namespace std;

Dup::Dup(){}

void Dup::ejecutar(Programa* programa){
    int valor = programa->getPila().top();
    programa->getPila().push(valor);
    programa->inc();
}

string Dup::mostrar(){
    return "dup";
}
