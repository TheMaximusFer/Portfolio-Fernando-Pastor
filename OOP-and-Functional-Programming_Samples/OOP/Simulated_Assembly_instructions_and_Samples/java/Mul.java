public class Mul extends Instruccion{

    public void ejecutar(Programa programa){

        int c = programa.getPila().pop();
        int h = programa.getPila().pop();
        programa.getPila().push(c*h);

        programa.inc();
    }

    public String mostrar(){
        return "mul";
    }
}