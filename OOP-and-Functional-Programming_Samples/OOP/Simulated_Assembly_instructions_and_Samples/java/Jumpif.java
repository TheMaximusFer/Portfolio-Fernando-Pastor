public class Jumpif extends Instruccion{

    private int c;

    public Jumpif(int c){
        this.c = c;
    }

    public void ejecutar(Programa programa){
        int h = programa.getPila().pop();
        if (h >= 0){
            programa.salto(c);
        }else{
            programa.inc();
        }
    }

    public String mostrar(){
        return "jumpif " + c;
    }
}