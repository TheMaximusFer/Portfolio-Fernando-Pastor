import java.util.Scanner;
import java.util.Stack;

public class Read extends Instruccion{

    private static final Scanner scanner = new Scanner(System.in);
    
    public void ejecutar(Programa programa){
        Stack<Integer> pila = programa.getPila();
        System.out.print("? ");
        int a = scanner.nextInt();
        pila.push(a);
        programa.inc();
    }

    public String mostrar(){
        return "read";
    }
}
