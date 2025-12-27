public class ExcepcionArbolFicheros extends Exception{
    public ExcepcionArbolFicheros(String mensaje){
        super(mensaje);
    }

    @Override
    public String toString(){
        return "ExcepcionArbolFicheros: " + getMessage();
    }
}
