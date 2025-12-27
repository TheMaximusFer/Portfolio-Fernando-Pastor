
#ifndef INSTRUCCION
#define INSTRUCCION

#include <string>
using namespace std;

class Programa;

class Instruccion{
public:
    virtual void ejecutar(Programa* programa) = 0;
    virtual string mostrar() = 0;
    virtual ~Instruccion(){}
};

#endif
