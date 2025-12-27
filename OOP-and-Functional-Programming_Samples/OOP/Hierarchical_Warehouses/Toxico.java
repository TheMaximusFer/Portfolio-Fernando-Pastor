public class Toxico implements CargaEspecial{
    private String nombre;
    private double volumen;
    private double peso;
    private String tabs = "";

    public Toxico(String nombre, double volumen, double peso) {
        this.nombre = nombre;
        this.volumen = volumen;
        this.peso = peso;
    }

    public String nombre() {
        return nombre;
    }

    public double volumen() {
        return volumen;
    }

    public double peso() {
        return peso;
    }

    public String tipoCarga(){
        return "Productos Tóxicos";
    }

    public void obtener_tabs(String tabs){
        this.tabs = tabs;
    }

    public String toString(){
        return tabs + nombre() + " [" + volumen() + " m3] [" + peso() + " kg]\n"; 
    }

}
