#ifndef WRITE
#define WRITE

#include "Instruccion.h"
#include "Programa.h"
using namespace std;

class Write : public Instruccion{
public:
    void ejecutar(Programa* programa) override;
    string mostrar() override;
};

#endif
