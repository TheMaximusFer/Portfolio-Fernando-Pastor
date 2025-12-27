#ifndef ADD
#define ADD

#include "Instruccion.h"
#include "Programa.h"
using namespace std;

class Add : public Instruccion{
public:
    void ejecutar(Programa* programa) override;
    string mostrar() override;
};

#endif
