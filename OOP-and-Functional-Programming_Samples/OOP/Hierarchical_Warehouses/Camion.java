public class Camion extends Almacen<Carga>{

    private static final String nombre = "Camion"; //constante
    
    public Camion(double volumen){
        super(volumen);
    }

    public String nombre(){
        return nombre;
    }

}