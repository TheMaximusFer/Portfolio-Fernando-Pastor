import java.util.List;
import java.util.ArrayList;

public class Shell{
    private Directorio root;
    private List<Directorio> ruta;

    public Shell(){
        this.root = new Directorio("");
        this.ruta = new ArrayList<>();
        this.ruta.add(root);
    }
    
    private Directorio obtenerDir(){
        return ruta.get(ruta.size() - 1);
    }

    private boolean existe(Directorio dir, String nombre){
        Nodo nodo = dir.obtenerNodo(nombre);
        return nodo != null;
    }

    public Directorio obtenerRoot(){
        return root;
    }

    public List<Directorio> obtenerRuta(){
        return ruta;
    }

    public String pwd(){
        if (ruta.size() == 1) return "/";
        String path = "";
        for (int i = 1; i < ruta.size(); i++){
            path = path + "/" + ruta.get(i).obtenerNombre();
        }
        return path;
    }
    
    public String ls(){
        List<String> nodos = obtenerDir().mostrar();
        String result = "";
        for (int i = 0; i < nodos.size(); i++){
            result = result + nodos.get(i);
            if (i < nodos.size() - 1){
                result = result + "\n";
            }
        }
        return result + "\n";
    }
    
    public String du(){
        Directorio actual = obtenerDir();
        List<String> nombres = actual.mostrar();
        String result = "";
        for (int i = 0; i < nombres.size(); i++){
            String nombreNodo = nombres.get(i);
            Nodo nodo = actual.obtenerNodo(nombreNodo);
            result = result + nodo.obtenerNombre() + " " + nodo.obtenerTam();
            if (i < nombres.size() - 1){
                result = result + "\n";
            }
        }
        return result + "\n";
    }
    

    public void vi(String nombre, int tamano) throws ExcepcionArbolFicheros{
        Directorio actual = obtenerDir();
        Nodo nodo = actual.obtenerNodo(nombre);
        if (nodo == null){
            nodo = new Fichero(nombre, tamano);
            actual.aniadir(nodo);
        } else{
            if (!(nodo instanceof Fichero))
                throw new TipoNodoIncorrectoException("vi", "Fichero", nodo.getClass().getSimpleName());
            nodo.modificarTam(tamano);
        }
    }

    public void mkdir(String nombre) throws ExcepcionArbolFicheros{
        Directorio actual = obtenerDir();
        if (existe(actual, nombre)){
            throw new NombreDuplicadoException(nombre);
        }
        actual.aniadir(new Directorio(nombre));
    }

    public void cd(String path) throws ExcepcionArbolFicheros{
        String[] tokens = path.split("/");
        List<Directorio> nueva = new ArrayList<>();

        if (path.startsWith("/")){
            nueva.add(root);
        }else{
            nueva.addAll(this.ruta);
        }
        
        for (String token : tokens){
            if (token.isEmpty() || token.equals(".")) continue;
            if (token.equals("..")){
                if (nueva.size() > 1){
                    nueva.remove(nueva.size() - 1);
                }
            }else{
                Directorio actual = nueva.get(nueva.size() - 1);
                Nodo nodo = actual.obtenerNodo(token);
                if (nodo == null){
                    throw new RutaInvalidaException(path);
                }
                if (nodo instanceof Enlace){
                    nodo = ((Enlace) nodo).obtenerRef();
                }
                if (!(nodo instanceof Directorio)){
                    throw new TipoNodoIncorrectoException("cd", "Directorio", nodo.getClass().getSimpleName());
                }
                nueva.add((Directorio) nodo);
            }
        }
        this.ruta = nueva;
    }

    public void ln(String rutaStr, String nombre) throws ExcepcionArbolFicheros{
        Directorio actual = obtenerDir();
        Nodo nodo = obtenerNodoPorRuta(rutaStr);
        if (nodo == null){
            throw new NodoNoEncontradoException(rutaStr);
        }
        if (existe(actual, nombre)){
            throw new NombreDuplicadoException(nombre);
        }
        actual.aniadir(new Enlace(nombre, nodo));
    }

    public int stat(String rutaStr) throws ExcepcionArbolFicheros{
        Nodo nodo = obtenerNodoPorRuta(rutaStr);
        if (nodo == null){
            throw new NodoNoEncontradoException(rutaStr);
        }
        return nodo.obtenerTam();
    }

    public void rm(String rutaStr) throws ExcepcionArbolFicheros{
        String[] partes = rutaStr.split("/");
        if (partes.length == 0){
            throw new RutaInvalidaException(rutaStr);
        }
        Directorio padre;
        if (rutaStr.startsWith("/")){
            padre = root;
        } else{
            padre = obtenerDir();
        }
        for (int i = 0; i < partes.length - 1; i++){
            if (partes[i].isEmpty() || partes[i].equals(".")) continue;
            if (partes[i].equals("..")){
                throw new RutaInvalidaException(rutaStr);
            }
            Nodo tmp = padre.obtenerNodo(partes[i]);
            if (tmp == null)
                throw new NodoNoEncontradoException(partes[i]);
            if (tmp instanceof Enlace){
                tmp = ((Enlace) tmp).obtenerRef();
            }
            if (!(tmp instanceof Directorio))
                throw new TipoNodoIncorrectoException("rm", "Directorio", tmp.getClass().getSimpleName());
            padre = (Directorio) tmp;
        }
        String nombreNodo = partes[partes.length - 1];
        if (nombreNodo.isEmpty())
            throw new RutaInvalidaException(rutaStr);
        if (padre.obtenerNodo(nombreNodo) == null)
            throw new NodoNoEncontradoException(nombreNodo);
        if (!padre.eliminar(nombreNodo))
            throw new ExcepcionArbolFicheros("No se pudo eliminar el nodo: " + nombreNodo);
    }

    //Método auxiliar para obtener un nodo a partir de una ruta (no soporta "..").
    private Nodo obtenerNodoPorRuta(String path){
        if (path.isEmpty()) return null;
        Directorio actual;
        if (path.startsWith("/")){
            actual = root;
        } else{
            actual = obtenerDir();
        }
        String[] tokens = path.split("/");
        Nodo nodo = null;
        for (String token : tokens){
            if (token.isEmpty() || token.equals(".")) continue;
            if (token.equals("..")){
                return null;
            }
            nodo = actual.obtenerNodo(token);
            if (nodo == null) return null;
            if (nodo instanceof Enlace){
                nodo = ((Enlace) nodo).obtenerRef();
            }
            if (nodo instanceof Directorio){
                actual = (Directorio) nodo;
            }
        }
        return nodo != null ? nodo : actual;
    }
}
