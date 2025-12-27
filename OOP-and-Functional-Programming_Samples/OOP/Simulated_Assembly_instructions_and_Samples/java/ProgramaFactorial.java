public class ProgramaFactorial extends Programa{
    public ProgramaFactorial(){
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
}
