public class TipoNodoIncorrectoException extends ExcepcionArbolFicheros{
    public TipoNodoIncorrectoException(String operacion, String tipoEsperado, String tipoReal){
        super("Operación " + operacion + " requiere " + tipoEsperado + " pero se encontró " + tipoReal);
    }
}
