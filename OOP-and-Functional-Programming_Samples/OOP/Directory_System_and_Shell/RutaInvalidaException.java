public class RutaInvalidaException extends ExcepcionArbolFicheros{
    public RutaInvalidaException(String ruta){
        super("Ruta inválida: " + ruta);
    }
}
