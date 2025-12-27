#include "ProgramaCuentaAtras.h"
#include "read.h"
#include "dup.h"
#include "write.h"
#include "push.h"
#include "add.h"
#include "jumpif.h"

ProgramaCuentaAtras::ProgramaCuentaAtras() : Programa(){
    anyadir(new Read());
    anyadir(new Dup());
    anyadir(new Write());
    anyadir(new Push(-1));
    anyadir(new Add());
    anyadir(new Dup());
    anyadir(new Jumpif(1));
}
