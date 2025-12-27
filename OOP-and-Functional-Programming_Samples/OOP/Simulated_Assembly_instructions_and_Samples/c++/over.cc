#include "over.h"
#include "Programa.h"
#include <stack>
using namespace std;

void Over::ejecutar(Programa* programa){
    stack<int>& pila = programa->getPila();
  
    int c = pila.top();
    pila.pop();
    int h = pila.top();
    pila.pop();

    pila.push(h);
    pila.push(c);
    pila.push(h);

    programa->inc();
}

string Over::mostrar(){
    return "over";
}
