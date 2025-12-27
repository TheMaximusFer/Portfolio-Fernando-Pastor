import java.util.Stack;

public class Add extends Instruccion{

    public void ejecutar(Programa programa){
        Stack<Integer> pila = programa.getPila();
        int a = pila.pop();
        int b = pila.pop();
        pila.push(a + b);
        programa.inc();
    }

    public String mostrar(){
        return "add";
    }
}
