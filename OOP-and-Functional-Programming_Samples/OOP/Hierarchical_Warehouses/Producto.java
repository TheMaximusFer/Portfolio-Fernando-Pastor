public class Producto implements Carga{
    protected double volumen;
    protected double peso;
    protected String nombre;
    private String tabs = "";

    public Producto(String nombre, double volumen, double peso){
        this.nombre = nombre;
        this.volumen = volumen;
        this.peso = peso;
    }

    public String nombre(){
        return nombre;
    }

    public double volumen(){
        return volumen;
    }

    public double peso(){
        return peso;
    }

    public String tipoCarga(){
        return "Carga Estándar";
    }

    public void obtener_tabs(String tabs){
        this.tabs = tabs;
    }

    public String toString(){
        return tabs + nombre() + " [" + volumen() + " m3] [" + peso() + " kg]\n"; 
    }

}