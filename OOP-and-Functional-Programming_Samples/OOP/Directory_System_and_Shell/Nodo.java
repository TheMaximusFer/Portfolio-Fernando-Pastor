public abstract class Nodo{
    protected String nombre;
    protected int tamano;

    public Nodo(String nombre, int tamano){
        this.nombre = nombre;
        this.tamano = tamano;
    }

    public String obtenerNombre(){
        return nombre;
    }

    public int obtenerTam(){
        return tamano;
    }
    public void modificarTam(int tamano){
        this.tamano = tamano;
    } 

}