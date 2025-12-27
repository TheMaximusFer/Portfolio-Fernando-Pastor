/* 
 *  La clase genérica, para evitar duplicación, gestiona el volumen (capacidad), el volumen_interno (carga que lleva),
 *  el peso correspondiente a la carga llevada y la visualización de dichas cargas.
 */

import java.util.ArrayList;
import java.util.List;

public abstract class Almacen<T extends AtributosBase>{

    private List<T> mercancia;
    private double volumen;
    private double volumen_interno;

    public Almacen(double volumen) {
        this.volumen = volumen;
        this.volumen_interno = 0;
        this.mercancia = new ArrayList<>();
    }

    abstract String nombre();

    public double volumen(){
        return volumen;    
    }

    public boolean guardar(T carga){
        if(volumen_interno + carga.volumen() > volumen){
            return false;
        }else{
            mercancia.add(carga);
            volumen_interno += carga.volumen();
            return true;
        }
    }

    public double peso() {
        double p = 0;
        for (T carga : mercancia) {
            p += carga.peso();
        }
        return p;
    }

    private String tabs_actuales = "";

    private String agnadir_tab(){
        tabs_actuales += "\t";
        return "";
    }

    private String quitar_tab(){
        if (tabs_actuales.length() > 0) {
            tabs_actuales = tabs_actuales.substring(0, tabs_actuales.length() - 1);
        }
        return "";
    }
    
    private String mostrar(){
        String p = "";
        for (int i = 0; i < mercancia.size() ; i++){
            mercancia.get(i).obtener_tabs(tabs_actuales);
            p += mercancia.get(i).toString();
        } 
        return p;       
    }

    public String toString(){
        switch(nombre()){
            case "Contenedor":
                return tabs_actuales + nombre() + " [" + volumen() + " m3] [" + peso() + " kg] de " + mercancia.get(0).tipoCarga() + "\n" + agnadir_tab() + mostrar() + quitar_tab();

            default:
                return nombre() + " [" + volumen() + " m3] [" + peso() + " kg]\n" + agnadir_tab() + mostrar() + quitar_tab();
        }
    }

    public void obtener_tabs(String tabs){
        this.tabs_actuales = tabs;
    }

    public String tipoCarga(){
        return "Carga Estándar";
    };
    
}
