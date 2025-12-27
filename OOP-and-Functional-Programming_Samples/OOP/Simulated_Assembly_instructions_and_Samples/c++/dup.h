#ifndef DUP
#define DUP

#include "Instruccion.h"
#include "Programa.h"
#include <string>
using namespace std;

class Dup : public Instruccion{
public:
    
    Dup();

    virtual void ejecutar(Programa* programa) override;
    virtual string mostrar() override;
};

#endif
