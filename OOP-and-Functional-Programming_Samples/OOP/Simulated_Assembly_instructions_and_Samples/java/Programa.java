import java.util.Stack;

public class Programa{

    private Instruccion[] instrucciones;
    private int contador;
    private int pc;
    private Stack<Integer> pila;
    
    public Programa(){
        this.instrucciones = new Instruccion[10]; //Ponemos una capacidad inicial de 10 instrucciones, por ejemplo.
        this.contador = 0;
        this.pc = 0;
        this.pila = new Stack<>();
    }
    
    //Añadimos instrucciones al vector dinámico. Si el vector está lleno, duplicamos su tamaño.
    public void anyadir(Instruccion instr){
        if (contador == instrucciones.length){
            Instruccion[] nuevoArray = new Instruccion[instrucciones.length * 2];
            for (int i = 0; i < instrucciones.length; i++){
                nuevoArray[i] = instrucciones[i];
            }
            instrucciones = nuevoArray;
        }
        instrucciones[contador++] = instr;
    }
    
    //Listamos las instrucciones del programa con su índice.
    public void listar(){
        for (int i = 0; i < contador; i++){
            System.out.println(i + ": " + instrucciones[i].mostrar());
        }
    }
    
    //Ejecutar el programa.
    public void ejecutar(){
        pila.clear();
        pc = 0;
        while (pc < contador){
            instrucciones[pc].ejecutar(this);
        }
    }
    
    //Incrementar pc.
    public void inc(){
        pc++;
    }
    
    //Cambiar valor de pc para dar un salto.
    public void salto(int nuevoPc){
        pc = nuevoPc;
    }

    public Stack<Integer> getPila(){
        return pila;
    }

}
