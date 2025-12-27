public class Contenedor<T extends CargaBase> extends Almacen<T> implements Carga{

    private String nombre = "Contenedor";

    public Contenedor(double volumen){
        super(volumen);
    }

    public String nombre(){
        return nombre;
    }

}

