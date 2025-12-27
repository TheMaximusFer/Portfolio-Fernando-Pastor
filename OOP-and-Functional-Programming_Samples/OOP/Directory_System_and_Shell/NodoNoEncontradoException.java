public class NodoNoEncontradoException extends ExcepcionArbolFicheros{
    public NodoNoEncontradoException(String nodo){
        super("Nodo no encontrado: " + nodo);
    }
}
