#ifndef PROGRAMA
#define PROGRAMA

#include <stack>
#include <iostream>
#include "Instruccion.h"
using namespace std;

class Programa{
private:

    Instruccion** instrucciones;
    int contador;
    int pc;
    int capacidad;
    stack<int> pila;

public:
    Programa();
    ~Programa();

    void anyadir(Instruccion* instr);
    void listar() const;
    void ejecutar();
    void inc();
    void salto(int nuevoPc);
    stack<int>& getPila();
};

#endif
