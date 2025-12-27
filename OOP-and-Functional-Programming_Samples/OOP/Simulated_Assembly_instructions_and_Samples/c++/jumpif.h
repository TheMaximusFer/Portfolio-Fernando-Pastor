#ifndef JUMPIF
#define JUMPIF

#include "Instruccion.h"
#include "Programa.h"
#include <string>
using namespace std;

class Jumpif : public Instruccion{
private:
    int c;
public:
    Jumpif(int c);
    virtual void ejecutar(Programa* programa) override;
    virtual string mostrar() override;
};

#endif
