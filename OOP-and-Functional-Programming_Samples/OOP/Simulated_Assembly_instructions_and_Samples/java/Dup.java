public class Dup extends Instruccion {

    public void ejecutar(Programa programa){
        int valor = programa.getPila().peek();
        programa.getPila().push(valor);
        programa.inc();
    }

    public String mostrar(){
        return "dup";
    }
}