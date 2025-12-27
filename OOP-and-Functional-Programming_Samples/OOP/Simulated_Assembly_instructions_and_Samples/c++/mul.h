#ifndef MUL
#define MUL

#include "Instruccion.h"
#include <string>
using namespace std;

class Mul : public Instruccion{
public:
    virtual void ejecutar(Programa* programa) override;
    virtual string mostrar() override;
};

#endif
