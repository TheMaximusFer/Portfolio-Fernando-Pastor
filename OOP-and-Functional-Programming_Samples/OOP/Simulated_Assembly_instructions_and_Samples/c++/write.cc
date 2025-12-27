#include "write.h"
#include <iostream>
using namespace std;

void Write::ejecutar(Programa* programa){
    stack<int>& pila = programa->getPila();
    if (!pila.empty()) {
        cout << "= " << pila.top() << endl;
        pila.pop();
    } else {
        cout << "No hay nada en la pila." << endl;
    }
    programa->inc();
}

string Write::mostrar() {
    return "write";
}
