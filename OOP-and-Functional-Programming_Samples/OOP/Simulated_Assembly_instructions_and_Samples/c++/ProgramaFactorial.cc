#include "ProgramaFactorial.h"
#include "push.h"
#include "read.h"
#include "swap.h"
#include "over.h"
#include "mul.h"
#include "add.h"
#include "dup.h"
#include "jumpif.h"
#include "write.h"

ProgramaFactorial::ProgramaFactorial() : Programa(){
    anyadir(new Push(1));
    anyadir(new Read());
    anyadir(new Swap());
    anyadir(new Over());
    anyadir(new Mul());
    anyadir(new Swap());
    anyadir(new Push(-1));
    anyadir(new Add());
    anyadir(new Dup());
    anyadir(new Push(-2));
    anyadir(new Add());
    anyadir(new Jumpif(2));
    anyadir(new Swap());
    anyadir(new Write());
}
