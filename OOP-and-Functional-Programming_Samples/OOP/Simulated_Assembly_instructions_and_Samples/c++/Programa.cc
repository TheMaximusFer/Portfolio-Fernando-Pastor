#include "Programa.h"
using namespace std;

Programa::Programa(){
    capacidad = 10;
    instrucciones = new Instruccion*[capacidad];
    contador = 0;
    pc = 0;
}

//Destructor.
Programa::~Programa(){
    for (int i = 0; i < contador; i++){
        delete instrucciones[i];
    }
    delete[] instrucciones;
}

//Añadimos instrucciones al vector dinámico. Si el vector está lleno, duplicamos su tamaño.
void Programa::anyadir(Instruccion* instr){
    if (contador == capacidad){
        int nuevaCapacidad = capacidad * 2;
        Instruccion** nuevoArray = new Instruccion*[nuevaCapacidad];
        for (int i = 0; i < capacidad; i++){
            nuevoArray[i] = instrucciones[i];
        }
        delete[] instrucciones;
        instrucciones = nuevoArray;
        capacidad = nuevaCapacidad;
    }
    instrucciones[contador++] = instr;
}

//Listamos las instrucciones del programa con su índice.
void Programa::listar() const{
    for (int i = 0; i < contador; i++){
        cout << i << ": " << instrucciones[i]->mostrar() << endl;
    }
}

//Ejecutar el programa.
void Programa::ejecutar(){
    while (!pila.empty()){
        pila.pop();
    }
    pc = 0;

    while (pc < contador){
        instrucciones[pc]->ejecutar(this);
    }
}

//Incrementar pc.
void Programa::inc(){
    pc++;
}

//Cambiar valor de pc para dar un salto.
void Programa::salto(int nuevoPc){
    pc = nuevoPc;
}

stack<int>& Programa::getPila(){
    return pila;
}
