#include "mul.h"
#include "Programa.h"
#include <stack>
using namespace std;

void Mul::ejecutar(Programa* programa){
    stack<int>& pila = programa->getPila();

    int c = pila.top();
    pila.pop();
    int h = pila.top();
    pila.pop();

    pila.push(c * h);
    programa->inc();
}

string Mul::mostrar(){
    return "mul";
}
