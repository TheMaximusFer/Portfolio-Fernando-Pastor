#ifndef SWAP
#define SWAP

#include "Instruccion.h"
#include <string>
using namespace std;

class Swap : public Instruccion{
public:
    virtual void ejecutar(Programa* programa) override;
    virtual string mostrar() override;
};

#endif
