import java.util.Stack;

public class Write extends Instruccion{

    public void ejecutar(Programa programa){
        Stack<Integer> pila = programa.getPila();
        if (!pila.isEmpty()){
            System.out.println("= " + pila.pop());
        } else{
            System.out.println("No hay nada en la pila.");
        }
        programa.inc();
    }

    public String mostrar(){
        return "write";
    }
}
