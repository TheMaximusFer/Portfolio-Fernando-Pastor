#include "read.h"
#include <iostream>
using namespace std;

void Read::ejecutar(Programa* programa){
    stack<int>& pila = programa->getPila();
    cout << "? ";
    int a;
    cin >> a;

    pila.push(a);
    programa->inc();
}

string Read::mostrar(){
    return "read";
}
