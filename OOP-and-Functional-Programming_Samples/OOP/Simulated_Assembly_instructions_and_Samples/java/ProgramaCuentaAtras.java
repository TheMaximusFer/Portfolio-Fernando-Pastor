public class ProgramaCuentaAtras extends Programa{
    public ProgramaCuentaAtras(){
        anyadir(new Read());
        anyadir(new Dup());
        anyadir(new Write());
        anyadir(new Push(-1));
        anyadir(new Add());
        anyadir(new Dup());
        anyadir(new Jumpif(1));
    }
}
