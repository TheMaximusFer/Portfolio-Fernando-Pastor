public class Push extends Instruccion{
    private int c;

    //Constructor del parámetro.
    public Push(int c){
        this.c = c;
    }

    public void ejecutar(Programa programa){
        programa.getPila().push(c);
        programa.inc();
    }

    public String mostrar(){
        return "push " + c;
    }
}