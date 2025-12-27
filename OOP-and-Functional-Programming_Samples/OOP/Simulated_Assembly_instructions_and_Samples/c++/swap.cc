#include "swap.h"
#include "Programa.h"
#include <stack>
using namespace std;

void Swap::ejecutar(Programa* programa){
    stack<int>& pila = programa->getPila();

    int c = pila.top();
    pila.pop();
    int h = pila.top();
    pila.pop();

    pila.push(c);
    pila.push(h);
    programa->inc();
}

string Swap::mostrar(){
    return "swap";
}
