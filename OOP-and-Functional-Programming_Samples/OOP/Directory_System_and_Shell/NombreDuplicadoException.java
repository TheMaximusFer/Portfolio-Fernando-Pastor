public class NombreDuplicadoException extends ExcepcionArbolFicheros{
    public NombreDuplicadoException(String nombre){
        super("Nombre duplicado: " + nombre);
    }
}
