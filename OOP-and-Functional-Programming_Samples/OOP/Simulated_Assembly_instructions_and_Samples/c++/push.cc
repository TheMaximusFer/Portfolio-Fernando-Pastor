#include "push.h"
using namespace std;

Push::Push(int c) : c(c){}

void Push::ejecutar(Programa* programa){
    programa->getPila().push(c);
    programa->inc();
}

string Push::mostrar(){
    return("push " + to_string(c));
}