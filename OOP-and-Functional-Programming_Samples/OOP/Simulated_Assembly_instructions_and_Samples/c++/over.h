#ifndef OVER
#define OVER

#include "Instruccion.h"
#include <string>
using namespace std;

class Over : public Instruccion{
public:
    virtual void ejecutar(Programa* programa) override;
    virtual string mostrar() override;
};

#endif
