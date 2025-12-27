#ifndef READ
#define READ

#include "Instruccion.h"
#include "Programa.h"
using namespace std;

class Read : public Instruccion{
public:
    void ejecutar(Programa* programa) override;
    string mostrar() override;
};

#endif
