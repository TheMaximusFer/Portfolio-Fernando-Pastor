#include <iostream>
#include "ProgramaSuma.h"
#include "ProgramaCuentaAtras.h"
#include "ProgramaFactorial.h"
using namespace std;

int main() {
    cout << "Ejecutando programa: Suma de dos números" << endl;
    ProgramaSuma suma;
    cout << "Listado:" << endl;
    suma.listar();
    cout << "Ejecucion:" << endl;
    suma.ejecutar();
    cout << "\n------------------------------\n" << endl;

    cout << "Ejecutando programa: Cuenta atras" << endl;
    ProgramaCuentaAtras cuentaAtras;
    cout << "Listado:" << endl;
    cuentaAtras.listar();
    cout << "Ejecucion:" << endl;
    cuentaAtras.ejecutar();
    cout << "\n------------------------------\n" << endl;

    cout << "Ejecutando programa: Factorial" << endl;
    ProgramaFactorial factorial;
    cout << "Listado:" << endl;
    factorial.listar();
    cout << "Ejecucion:" << endl;
    factorial.ejecutar();
    cout << "\n------------------------------\n" << endl;

    return 0;
}
