#include "add.h"
using namespace std;

void Add::ejecutar(Programa* programa){
    stack<int>& pila = programa->getPila();
    int a = pila.top();
    pila.pop();
    int b = pila.top();
    pila.pop();

    pila.push(a+b);

    programa->inc();
}

string Add::mostrar(){
    return "add";
}
