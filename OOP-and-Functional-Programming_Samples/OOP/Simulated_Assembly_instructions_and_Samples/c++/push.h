#ifndef PUSH
#define PUSH

#include "Instruccion.h"
#include "Programa.h"
#include <string>
using namespace std;

class Push : public Instruccion{
private:
    int c;
public:

    Push(int c);
    
    virtual void ejecutar(Programa* programa) override;
    virtual string mostrar() override;
};

#endif