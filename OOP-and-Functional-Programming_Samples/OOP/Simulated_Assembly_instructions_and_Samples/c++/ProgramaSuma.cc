#include "ProgramaSuma.h"
#include "read.h"
#include "add.h"
#include "write.h"

ProgramaSuma::ProgramaSuma() : Programa(){
    anyadir(new Read());
    anyadir(new Read());
    anyadir(new Add());
    anyadir(new Write());
}
