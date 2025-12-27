public class Enlace extends Nodo{
    private Nodo reference;

    public Enlace(String nombre, Nodo reference){
        super(nombre, 0); //El tamaño se determinará de forma dinámica
        this.reference = reference;
    }

    public int obtenerTam(){
        return reference.obtenerTam();
    }

    public Nodo obtenerRef(){
        return reference;
    }
}
