import java.util.List;
import java.util.ArrayList;

public class Directorio extends Nodo{
    private List<Nodo> dentro;

    public Directorio(String nombre){
        super(nombre, 0);
        this.dentro = new ArrayList<>();
    }

    public boolean aniadir(Nodo nodo){
        for (Nodo n : dentro){
            if (n.obtenerNombre().equals(nodo.obtenerNombre())){
                return false;
            }
        }
        dentro.add(nodo);
        return true;
    }

    public boolean eliminar(String nombre){
        for (int i = 0; i < dentro.size(); i++){
            if (dentro.get(i).obtenerNombre().equals(nombre)){
                dentro.remove(i);
                return true; 
            }
        }
        return false; 
    }

    public Nodo obtenerNodo(String nombre){
        for (Nodo n : dentro){
            if (n.obtenerNombre().equals(nombre)){
                return n;
            }
        }
        return null;
    } 

    public List<String> mostrar(){
        List<String> nombres = new ArrayList<>();
        for (Nodo n : dentro){
            nombres.add(n.obtenerNombre());
        }
        return nombres;
    }

    public int obtenerTam(){
        int suma = 0;
        for (Nodo n : dentro){
            suma += n.obtenerTam();
        }
        return suma;
    }
}
