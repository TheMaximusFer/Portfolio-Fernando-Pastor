/* 
* Autores: 
*    - Daniel Jal Burguete, 815450 
*    - Fernado Pastor Peralta, 897113
*/
#ifndef COLEC_INTERDEP_HPP
#define COLEC_INTERDEP_HPP

#include <string>
#include "pilaDinamica.hpp"

// --- INICIO DE LA PARTE PÚBLICA O INTERFAZ ---

/* 
 *  Los valores del TAD representan colecciones de elementos formados como tuplas  
 * de la forma (ident, val, -, NumDepend) o bien (ident, val, identSup, NumDepend). A los elementos  
 * con forma (ident, val, -, NumDepend) los llamaremos en general ‘elementos independientes’,  
 * mientras que a los elementos con forma (ident, val, identSup, NumDepend), los llamaremos en  
 * general ‘elementos dependientes’. En la colección no podrá haber dos elementos con el mismo  
 * ident. 
 * En las tuplas que representan elementos dependientes, la información identSup será la  
 * identificación del elemento del que es directamente dependiente el elemento con identificación  
 * ident. Ningún elemento de la colección podrá ser directamente dependiente de sí mismo, y todo  
 * elemento dependiente debe serlo de otro elemento que exista en la colección (que a su vez puede  
 * ser un elemento independiente o dependiente).  
 * En cada elemento, la información NumDepend de su tupla representará el número total de elementos  
 * en la colección que son directamente dependientes del elemento con identificador ident, y que  
 * será 0 si ningún elemento de la colección depende de dicho elemento.}
 */
template<typename I, typename V>
struct ColecInterdep;


// INTERFAZ DEL TAD coleccionesInterdependientes PRE-DECLARACIONES:
// OPERACIONES DEL TAD  coleccionesInterdependientes:

/*  Se requiere que el género ident tenga definidas las siguientes operaciones.
 *  Las operaciones igual y anterior definen una relación de orden total (i.e., permiten
 *  organizar los datos de la colección en forma de secuencia ordenada).
 */
template<typename I> bool igual(const I& s1, const I& s2);
template<typename I> bool anterior(const I& s1, const I& s2);

// Devuelve una colección vacía, sin elementos.
template<typename I, typename V> void crear(ColecInterdep<I, V>& c);

// Devuelve el número de elementos que hay en la colección c.
template<typename I, typename V> int tamaño(const ColecInterdep<I, V>& c);

// Devuelve verdad si y solo si c no contiene ningún elemento.
template<typename I, typename V> bool esVacia(const ColecInterdep<I, V>& c);

// Devuelve verdad si y solo si en c hay algún elemento con ident igual a id.
template<typename I, typename V> bool existe(const ColecInterdep<I, V>& c, const I& id);

/* Devuelve verdad si y solo si en c hay algún elemento dependiente cuyo ident sea igual a id, es  
   decir, un elemento (id, v, idSup, NumDep).*/
template<typename I, typename V> bool existeDependiente(const ColecInterdep<I, V>& c, const I& id);

/* Devuelve verdad si y solo si en c hay algún elemento independiente cuyo ident sea igual a id, es  
   decir, un elemento (id, v, -, NumDep).*/
template<typename I, typename V> bool existeIndependiente(const ColecInterdep<I, V>& c, const I& id);

/* Si no existe?(id,c), devuelve una colección igual a la resultante de añadir el elemento  
  independiente (id,v,-,0) a la colección c. En caso contrario, devuelve una colección igual a c.*/
template<typename I, typename V> void añadirIndependiente(ColecInterdep<I, V>& c, const I& id, const V& v);

/* Si no existe?(id,c) y existe?(super,c) devuelve una colección igual a la resultante de:  
  incrementar en 1 el número de elementos dependientes del elemento con identificador super en c, y  
  de añadir el elemento (id,v,super,0) a la colección c.  
  En cualquier otro caso, devuelve una colección igual a c.*/
template<typename I, typename V> void añadirDependiente(ColecInterdep<I, V>& c, const I& id, const V& v, const I& super);

/* Si no igual(id,super) y existe?(super,c) y existeDependiente?(id,c), sea su forma  
  (id, v, superAnt, NumDep), devuelve una colección igual a la resultante de: incrementar en 1 el  
  número de elementos dependientes del elemento con identificador super en c, decrementar en 1 el  
  número de elementos dependientes del elemento con identificador superAnt en c, y sustituir el  
  elemento (id, v, superAnt, NumDep) por el elemento (id, v, super, NumDep) en c.  
  Si no igual(id,super) y existe?(super,c) y existeIndependiente?(id,c), sea su forma  
  (id, v, -, NumDep), devuelve una colección igual a la resultante de: incrementar en 1 el número  
  de elementos dependientes del elemento con identificador super en c, y sustituir en c el elemento  
  (id, v, -, NumDep) por el elemento (id, v, super, NumDep).  
  En cualquier otro caso, devuelve una colección igual a c.*/
template<typename I, typename V> void hacerDependiente(ColecInterdep<I, V>& c, const I& id, const I& super);
/* Si existeDependiente?(id,c), sea su forma (id, v, superAnt, NumDep), devuelve una colección igual 
   a la resultante de: decrementar en 1 el número de elementos dependientes del elemento con
   identificador superAnt en c, y sustituir el elemento (id, v, superAnt, NumDep) por el elemento
   (id, v, -, NumDep). En cualquier otro caso, devuelve una colección igual a c.*/
template<typename I, typename V> void hacerIndependiente(ColecInterdep<I, V>& c, const I& id);

/* Si existeIndependiente?(id,c), sea su forma (id, v, -, NumDep), devuelve una colección igual a
   la resultante de sustituir el elemento (id, v, -, NumDep) por el elemento (id, nuevo, -, NumDep)
   en la colección c. Si existeDependiente?(id,c), sea su forma (id, v, super, NumDep), devuelve una
   colección igual a la resultante de sustituir el elemento (id, v, super, NumDep) por el elemento
   (id, nuevo, super, NumDep) en la colección c.
   Parcial: la operación no está definida si no existe?(id,c).*/
template<typename I, typename V> bool actualizarVal(ColecInterdep<I, V>& c, const I& id, const V& nuevo);

/* Si en c hay algún elemento con ident igual a id, sea (id,v,-,num) o sea (id,v,sup,num), devuelve
    el dato v que forma parte del elemento con dicho id.
    Parcial: la operación no está definida si no existe?(id,c).*/
template<typename I, typename V> bool obtenerVal(const ColecInterdep<I, V>& c, const I& id, V& resultado);

/* Si existeDependiente?(id,c), sea su forma (id, v, sup, NumDep), devuelve el dato sup que forma
   parte del elemento con dicho id.
   Parcial: la operación no está definida si no existeDependiente?(id,c).*/
template<typename I, typename V> bool obtenerSupervisor(const ColecInterdep<I, V>& c, const I& id, I& resultado);

/* Si existe?(id,c), sea su forma (id, v, -, NumDep) o (id, v, sup, NumDep), devuelve el dato NumDep
   que forma parte del elemento con dicho id. 
   Parcial: la operación no está definida si no existe?(id,c).*/
template<typename I, typename V> bool obtenerNumDependientes(const ColecInterdep<I, V>& c, const I& id, int& resultado);

/* Si existeDependiente?(id,c), sea su forma (id, v, sup, NumDep), y obtenerNúmDependientes(id,c)=0,
   devuelve una colección igual a la resultante de: decrementar en 1 el número de elementos
   dependientes del elemento con identificador sup, y eliminar el elemento (id, v, sup, NumDep), en
   la colección c.
   Si existeIndependiente?(id,c), sea su forma (id, v, -, NumDep), y obtenerNúmDependientes(id,c)=0,
   devuelve una colección igual a la resultante de eliminar el elemento (id, v, -, NumDep) en la  
   colección c. En cualquier otro caso, devuelve una colección igual a c.*/
template<typename I, typename V> void borrar(ColecInterdep<I, V>& c, const I& id);

// Borra todos los elementos de la colección c (si hay), dejándola vacía.
template<typename I, typename V> void borrarTodo(ColecInterdep<I, V>& c);


//OPERACIONES ITERADOR
/* Las siguientes operaciones constituyen un iterador que permite visitar los elementos de la
   colección de forma que siempre se visiten antes los elementos con ident anteriores a los que se
   visitarán más tarde:*/

/* Inicializa el iterador para recorrer los elementos de la colección c, de forma que el siguiente
   elemento a visitar sea el que tiene un ident anterior a los de todos los demás elementos de la
   colección (situación de no haber visitado ningún elemento).*/
template<typename I, typename V> void iniciarIterador(ColecInterdep<I, V>& c);

/* Devuelve verdad si queda algún elemento por visitar con el iterador de la colección c, devuelve
   falso si ya se ha visitado el último elemento.*/
template<typename I, typename V> bool existeSiguiente(const ColecInterdep<I, V>& c);

/* Devuelve el ident del siguiente elemento a visitar con el iterador de la colección c, que será
   el elemento no visitado con ident anterior a los de todos los demás aún no visitados.
   Parcial: la operación no está definida si no quedan elementos por visitar (no existeSiguiente?(c))*/
template<typename I, typename V> bool siguienteIdent(const ColecInterdep<I, V>& c, I& resultado);

/* Devuelve el val del siguiente elemento a visitar con el iterador de la colección c, que será el
   elemento no visitado con ident anterior a los de todos los demás aún no visitados.
   Parcial: la operación no está definida si no quedan elementos por visitar (no existeSiguiente?(c))*/
template<typename I, typename V> bool siguienteVal(const ColecInterdep<I, V>& c, V& resultado);

/* Si el siguiente elemento a visitar con el iterador de la colección, que será el elemento no
   visitado con ident anterior a los de todos los demás aún no visitados, es de la forma 
   (ident, val, -, numDep) devuelve falso, pero si es de la forma (ident, val, identSup, numDep)
   devuelve verdad.
   Parcial: la operación no está definida si no quedan elementos por visitar (no existeSiguiente?(c))*/
template<typename I, typename V> bool siguienteDependiente(const ColecInterdep<I, V>& c);

/* Si el siguiente elemento a visitar con el iterador de la colección, que será el elemento no
   visitado con ident anterior a los de todos los demás aún no visitados, es de la forma
   (ident, val, identSup, numDep), devuelve su identSup.
   Parcial: la operación no está definida si no quedan elementos por visitar (no existeSiguiente?(c)),
   o bien si existeSiguiente?(c) y no es verdad siguienteDependiente?(c).*/
template<typename I, typename V> bool siguienteSuperior(const ColecInterdep<I, V>& c, I& resultado);

/*Devuelve el NumDep del siguiente elemento a visitar con el iterador de la colección c, qu
  será el elemento no visitado con ident anterior a los de todos los demás aún no visitados.
  Parcial: la operación no está definida si no quedan elementos por visitar (no existeSiguiente?(c))*/
template<typename I, typename V> bool siguienteNumDependientes(const ColecInterdep<I, V>& c, int& resultado);

/* Avanza el iterador de la colección c para que se pueda visitar otro elemento.
   Parcial: la operación no está definida si no quedan elementos por visitar (no existeSiguiente?(c))*/
template<typename I, typename V> void avanza(ColecInterdep<I, V>& c);

//FIN DE LAS OPERACIONES DEL TAD

//DECLARACIÓN DEL TAD  coleccionesInterdependientes:
template<typename I, typename V>
struct ColecInterdep {
    /*
    * Declaración de la representación interna del tipo:
    * Tiene tres componentes principales:
    *   1. puntero a la raíz del árbol binario de búsqueda.
    *   2. pila de punteros a nodo para el iterador.
    *   3. contador de elementos para eficiencia O(1).
    * Cada nodo está formado por:
    *   1. ident: identificador del elemento.
    *   2. val: valor asociado al elemento.
    *   3. esDependiente: booleano que indica si el elemento es dependiente o independiente.
    *   4. identSup: identificador del elemento del que depende (si es dependiente).
    *   5. numDepend: número de elementos que dependen directamente de este elemento.
    *   6. izq: puntero al nodo hijo izquierdo.
    *   7. der: puntero al nodo hijo derecho.
    */
    private:
        struct Nodo {
            I ident;
            V val;
            bool esDependiente;
            I identSup; // Guardamos el supervisor como identificador, con el borrado por copia no tenemos que actualizar punteros de dependientes y mantenemos el coste de borrado en O(h).
            int numDepend;
            Nodo* izq;
            Nodo* der;
        };

        Nodo* raiz;
        Pila<Nodo*> iterador;
        int numElementos;

        //Métodos Auxiliares Privados.
        static Nodo* buscarNodoRec(Nodo* n, const I& id);
        static const Nodo* buscarNodoRec(const Nodo* n, const I& id);
        static void insertarNodo(Nodo*& n, Nodo* nuevo);
        static void borrarNodoRec(Nodo*& n, const I& id);
        static Nodo* buscarMaximo(Nodo* n);
        static void liberarArbol(Nodo*& n);
        static void apilarRamaIzquierda(Pila<Nodo*>& p, Nodo* n);
        static Nodo* obtenerCima(const ColecInterdep<I, V>& c);

        //Helpers de instancia.
        Nodo* buscarNodo(const I& id);
        const Nodo* buscarNodo(const I& id) const;

    public:
        friend void crear<I, V>(ColecInterdep<I, V>& c);
        friend int tamaño<I, V>(const ColecInterdep<I, V>& c);
        friend bool esVacia<I, V>(const ColecInterdep<I, V>& c);
        friend bool existe<I, V>(const ColecInterdep<I, V>& c, const I& id);
        friend bool existeDependiente<I, V>(const ColecInterdep<I, V>& c, const I& id);
        friend bool existeIndependiente<I, V>(const ColecInterdep<I, V>& c, const I& id);
        friend void añadirIndependiente<I, V>(ColecInterdep<I, V>& c, const I& id, const V& v);
        friend void añadirDependiente<I, V>(ColecInterdep<I, V>& c, const I& id, const V& v, const I& super);
        friend void hacerDependiente<I, V>(ColecInterdep<I, V>& c, const I& id, const I& super);
        friend void hacerIndependiente<I, V>(ColecInterdep<I, V>& c, const I& id);
        friend bool actualizarVal<I, V>(ColecInterdep<I, V>& c, const I& id, const V& nuevo);
        friend bool obtenerVal<I, V>(const ColecInterdep<I, V>& c, const I& id, V& resultado);
        friend bool obtenerSupervisor<I, V>(const ColecInterdep<I, V>& c, const I& id, I& resultado);
        friend bool obtenerNumDependientes<I, V>(const ColecInterdep<I, V>& c, const I& id, int& resultado);
        friend void borrar<I, V>(ColecInterdep<I, V>& c, const I& id);
        friend void borrarTodo<I, V>(ColecInterdep<I, V>& c);
        //OPERACIONES DEL ITERADOR
        friend void iniciarIterador<I, V>(ColecInterdep<I, V>& c);
        friend bool existeSiguiente<I, V>(const ColecInterdep<I, V>& c);
        friend bool siguienteIdent<I, V>(const ColecInterdep<I, V>& c, I& resultado);
        friend bool siguienteVal<I, V>(const ColecInterdep<I, V>& c, V& resultado);
        friend bool siguienteDependiente<I, V>(const ColecInterdep<I, V>& c);
        friend bool siguienteSuperior<I, V>(const ColecInterdep<I, V>& c, I& resultado);
        friend bool siguienteNumDependientes<I, V>(const ColecInterdep<I, V>& c, int& resultado);
        friend void avanza<I, V>(ColecInterdep<I, V>& c);
};

// --- FIN DE LA PARTE PÚBLICA o INTERFAZ ---



// IMPLEMENTACIÓN DE LAS FUNCIONES DEL TAD coleccionesInterdependientes:

// --- Operaciones auxiliares (no parte de la especificación pública) ---

/*
 * Pre: n es la raíz de un subárbol (posiblemente vacío) ordenado por identificador.
 * Post: Devuelve un puntero al nodo con identificador igual a id si existe en el subárbol.
 * Devuelve nullptr si no existe. 
 * 
 * Busca recursivamente un nodo por su identificador en el ABB.
 * Complejidad: O(h) donde h es la altura del árbol (O(log n) en promedio, O(n) peor caso).
 */
template<typename I, typename V>
typename ColecInterdep<I, V>::Nodo* ColecInterdep<I, V>::buscarNodoRec(Nodo* n, const I& id) {
    if (n == nullptr) return nullptr;  //Caso base: Si el nodo es nulo, hemos llegado al final de una rama sin encontrar el elemento.
    if (igual(id, n->ident)) return n; // Caso base: Si el identificador coincide con el del nodo actual, hemos encontrado el elemento.
    if (anterior(id, n->ident)) return buscarNodoRec(n->izq, id); //Si el id buscado es anterior (menor), buscamos en el subárbol izquierdo.
    return buscarNodoRec(n->der, id);  //Si no es igual ni anterior, debe ser posterior (mayor), buscamos en el subárbol derecho.
}

/*
 * Versión CONST de buscarNodoRec
 */
template<typename I, typename V>
const typename ColecInterdep<I, V>::Nodo* ColecInterdep<I, V>::buscarNodoRec(const Nodo* n, const I& id) {
    if (n == nullptr) return nullptr;
    if (igual(id, n->ident)) return n;
    if (anterior(id, n->ident)) return buscarNodoRec(n->izq, id);
    return buscarNodoRec(n->der, id);
}

/*
 * Pre: ---
 * Post: Devuelve un puntero al nodo con ident igual a id, o nullptr si no existe.
 * 
 * Método auxiliar de instancia que llama a buscarNodoRec.
 * Complejidad: O(h).
 */
template<typename I, typename V>
typename ColecInterdep<I, V>::Nodo* ColecInterdep<I, V>::buscarNodo(const I& id) {
    return buscarNodoRec(this->raiz, id);
}

/*
 * Versión CONST de buscarNodo (Helper de instancia)
 */
template<typename I, typename V>
const typename ColecInterdep<I, V>::Nodo* ColecInterdep<I, V>::buscarNodo(const I& id) const {
    return buscarNodoRec(this->raiz, id);
}


/*
 * Pre: n es la raíz de un subárbol ordenado, 'nuevo' es un nodo válido no nulo.
 * Post: Inserta el nodo 'nuevo' en la posición correcta del subárbol apuntado por n para mantener el orden.
 * Si n era nullptr, n pasa a apuntar a 'nuevo'.
 * 
 * Complejidad: O(h) donde h es la altura del árbol.
 */
template<typename I, typename V>
void ColecInterdep<I, V>::insertarNodo(Nodo*& n, Nodo* nuevo) {
    if (n == nullptr) { //Caso base: Hemos llegado a un hueco libre en el árbol.
        n = nuevo;
    } else if (anterior(nuevo->ident, n->ident)) { //Si el identificador es menor (anterior), descendemos recursivamente por la izquierda.
        insertarNodo(n->izq, nuevo);
    } else { //En caso contrario (mayor), descendemos recursivamente por la derecha.
        insertarNodo(n->der, nuevo);
    }
}

/*
 * Pre: n es la raíz de un subárbol no vacío.
 * Post: Devuelve un puntero al nodo con el identificador mayor dentro del subárbol.
 * 
 * Complejidad: O(h) donde h es la altura del subárbol.
 */
template<typename I, typename V>
typename ColecInterdep<I, V>::Nodo* ColecInterdep<I, V>::buscarMaximo(Nodo* n) {
    //Busca el nodo con el máximo identificador en un subárbol (el más a la derecha).
    while (n->der != nullptr) {
        n = n->der;
    }
    return n;
}

/*
 * Pre: n es la raíz de un subárbol ordenado.
 * Post: Si id existe en el subárbol, elimina el nodo correspondiente reestructurando el árbol 
 * para mantener el orden (usando el predecesor en caso de dos hijos) y libera su memoria.
 * Si id no existe, el subárbol no se modifica.
 * 
 * Borra recursivamente un nodo del ABB manteniendo la estructura del árbol.
 * Usa el predecesor inorden (máximo del subárbol izquierdo) cuando hay dos hijos.
 * Complejidad: O(h) donde h es la altura del árbol.
 */
template<typename I, typename V>
void ColecInterdep<I, V>::borrarNodoRec(Nodo*& n, const I& id) {
    if (n == nullptr) return;

    if (anterior(id, n->ident)) {
        borrarNodoRec(n->izq, id);
    } else if (anterior(n->ident, id)) {
        borrarNodoRec(n->der, id);
    } else {
        //Nodo encontrado, procedemos a borrar.
        Nodo* aBorrar = n;
        if (n->izq == nullptr) {
            n = n->der; //Enlazamos con el hijo derecho.
            delete aBorrar;
        } else if (n->der == nullptr) {
            n = n->izq; //Enlazamos con el hijo izquierdo.
            delete aBorrar;
        } else {
            //Caso con dos hijos: Buscamos el predecesor y lo extraemos eficientemente.
            //Si el hijo izquierdo no tiene hijo derecho, él es el predecesor.
            if (n->izq->der == nullptr) {
                Nodo* predecesor = n->izq;
                //Copiamos los datos del predecesor al nodo actual.
                n->ident = predecesor->ident;
                n->val = predecesor->val;
                n->esDependiente = predecesor->esDependiente;
                n->identSup = predecesor->identSup;
                n->numDepend = predecesor->numDepend;
                //El hijo izquierdo ahora apunta al hijo izquierdo del predecesor.
                n->izq = predecesor->izq;
                delete predecesor;
            } else {
                //Buscamos el máximo del subárbol izquierdo (el más a la derecha).
                Nodo* padre = n->izq;
                while (padre->der->der != nullptr) {
                    padre = padre->der;
                }
                Nodo* predecesor = padre->der;
                //Copiamos los datos del predecesor al nodo actual.
                n->ident = predecesor->ident;
                n->val = predecesor->val;
                n->esDependiente = predecesor->esDependiente;
                n->identSup = predecesor->identSup;
                n->numDepend = predecesor->numDepend;
                //Enlazamos el padre con el hijo izquierdo del predecesor.
                padre->der = predecesor->izq;
                delete predecesor;
            }
        }
    }
}

/*
 * Pre: n es raíz de un subárbol.
 * Post: Libera la memoria de todos los nodos del subárbol y asigna nullptr a n.
 * 
 * Complejidad: O(n) donde n es el número de nodos.
 */
template<typename I, typename V>
void ColecInterdep<I, V>::liberarArbol(Nodo*& n) {
    if (n != nullptr) {
        //Libera recursivamente todos los nodos del árbol. Recorrido Post-Orden.
        liberarArbol(n->izq);
        liberarArbol(n->der);
        delete n;
        n = nullptr;
    }
}

/*
 * Pre: p es una pila de nodos inicializada, n es un nodo del árbol.
 * Post: Apila en p todos los nodos de la rama izquierda comenzando desde n (para recorrido inorden).
 * 
 * Complejidad: O(h) donde h es la altura desde n.
 */
template<typename I, typename V>
void ColecInterdep<I, V>::apilarRamaIzquierda(Pila<Nodo*>& p, Nodo* n) {
    while (n != nullptr) { 
        apilar(p, n);
        n = n->izq;
    }
}

/*
 * Pre: El iterador de la colección c está inicializado.
 * Post: Devuelve un puntero al nodo de la cima de la pila.
 * Devuelve nullptr si la pila está vacía o hay error.
 * 
 * Complejidad: O(1).
 */
template<typename I, typename V>
typename ColecInterdep<I, V>::Nodo* ColecInterdep<I, V>::obtenerCima(const ColecInterdep<I, V>& c) {
    Nodo* n = nullptr;
    bool error;
    cima(c.iterador, n, error);
    return n;
}


// --- OPERACIONES DEL TAD COLECINTERDEP ---

/*
 * Pre:  c debe estar inicializada.
 * Post: Devuelve el número de elementos en c.
 * 
 * Complejidad: O(1).
 */
template<typename I, typename V>
void crear(ColecInterdep<I, V>& c) {
    c.raiz = nullptr;
    c.numElementos = 0;
    crearVacia(c.iterador);
}

/*
 * Pre:  c debe estar inicializada.
 * Post: Devuelve el número de elementos en c.
 * 
 * Complejidad: O(1) - usa contador en lugar de iterar.
 */
template<typename I, typename V>
int tamaño(const ColecInterdep<I, V>& c) {
    return c.numElementos;
}

/*
 * Pre: c debe estar inicializada.
 * Post: Devuelve true si y solo si c no contiene ningún elemento.
 * 
 * Complejidad: O(1).
 */
template<typename I, typename V>
bool esVacia(const ColecInterdep<I, V>& c) {
    return c.numElementos == 0;
}

/*
 * Pre: c debe estar inicializada.
 * Post: Devuelve verdad si existe un nodo con ident igual a id.
 * 
 * Complejidad: O(h) donde h es la altura del árbol.
 */
template<typename I, typename V>
bool existe(const ColecInterdep<I, V>& c, const I& id) {
    return c.buscarNodo(id) != nullptr;
}

/*
 * Pre: c debe estar inicializada.
 * Post: Devuelve verdad si existe un nodo con ident igual a id y es dependiente.
 * 
 * Complejidad: O(h).
 */
template<typename I, typename V>
bool existeDependiente(const ColecInterdep<I, V>& c, const I& id) {
    const typename ColecInterdep<I, V>::Nodo* n = c.buscarNodo(id);
    return n != nullptr && n->esDependiente;
}

/*
 * Pre: c debe estar inicializada.
 * Post: Devuelve verdad si existe un nodo con ident igual a id y es independiente.
 * 
 * Complejidad: O(h).
 */
template<typename I, typename V>
bool existeIndependiente(const ColecInterdep<I, V>& c, const I& id) {
    const typename ColecInterdep<I, V>::Nodo* n = c.buscarNodo(id);
    return n != nullptr && !n->esDependiente;
}

/*
 * Pre: c debe estar inicializada.
 * Post: Añade un nodo independiente si no existe id, insertándolo en orden.
 * 
 * Complejidad: O(h) - una sola búsqueda + inserción.
 */
template<typename I, typename V>
void añadirIndependiente(ColecInterdep<I, V>& c, const I& id, const V& v) {

    typename ColecInterdep<I, V>::Nodo* nodoExistente = c.buscarNodo(id);
    
    //Solo procedemos si el identificador es único en la colección.
    if (nodoExistente == nullptr) {
        typename ColecInterdep<I, V>::Nodo* nuevo = new typename ColecInterdep<I, V>::Nodo;
        nuevo->ident = id;
        nuevo->val = v;
        nuevo->esDependiente = false;
        nuevo->numDepend = 0;
        nuevo->izq = nullptr;
        nuevo->der = nullptr;
        
        ColecInterdep<I, V>::insertarNodo(c.raiz, nuevo);
        c.numElementos++;
    }
}

/*
 * Pre: c debe estar inicializada.
 * Post: Hace dependiente el nodo con id a super, actualizando numDepend si es necesario.
 * 
 * Complejidad: O(h) - dos búsquedas + inserción, todo O(h).
 */
template<typename I, typename V>
void añadirDependiente(ColecInterdep<I, V>& c, const I& id, const V& v, const I& super) {
    //Buscar el supervisor.
    typename ColecInterdep<I, V>::Nodo* supNode = c.buscarNodo(super);
    
    typename ColecInterdep<I, V>::Nodo* nodoExistente = c.buscarNodo(id);
    
    //El supervisor debe existir y el nuevo id no debe existir.
    if (supNode != nullptr && nodoExistente == nullptr) {
        typename ColecInterdep<I, V>::Nodo* nuevo = new typename ColecInterdep<I, V>::Nodo;
        nuevo->ident = id;
        nuevo->val = v;
        nuevo->esDependiente = true;
        nuevo->identSup = super;
        nuevo->numDepend = 0;
        nuevo->izq = nullptr;
        nuevo->der = nullptr;

        ColecInterdep<I, V>::insertarNodo(c.raiz, nuevo);
        supNode->numDepend++;
        c.numElementos++;
    }
}

/*
 * Pre: c debe estar inicializada.
 * Post: Hace dependiente el nodo con id a super, actualizando numDepend si es necesario.
 * 
 * Usa punteros directos para evitar búsquedas adicionales al decrementar.
 * Complejidad: O(h) - dos búsquedas, sin búsqueda extra del supervisor antiguo.
 */
template<typename I, typename V>
void hacerDependiente(ColecInterdep<I, V>& c, const I& id, const I& super) {
    if (igual(id, super)) return; //Un elemento no puede depender de sí mismo.

    typename ColecInterdep<I, V>::Nodo* nodoId = c.buscarNodo(id);
    typename ColecInterdep<I, V>::Nodo* nodoSup = c.buscarNodo(super);

    //Solo procedemos si ambos elementos (dependiente y nuevo supervisor) existen.
    if (nodoId != nullptr && nodoSup != nullptr) {
        //Si el nodo ya era dependiente, hay que actualizar al supervisor anterior.
        if (nodoId->esDependiente) {
            typename ColecInterdep<I, V>::Nodo* oldSup = c.buscarNodo(nodoId->identSup);
            if (oldSup != nullptr) {
                oldSup->numDepend--; //Decrementamos el contador del antiguo supervisor.
            }
        }
        nodoId->esDependiente = true;
        nodoId->identSup = super; 
        nodoSup->numDepend++; //Incrementamos el contador del nuevo supervisor.
    }
}

/*
 * Pre: c debe estar inicializada.
 * Post: Hace independiente el nodo con id si es dependiente, decrementando numDepend de su super.
 * 
 * Complejidad: O(h) - dos búsquedas.
 */
template<typename I, typename V>
void hacerIndependiente(ColecInterdep<I, V>& c, const I& id) {
typename ColecInterdep<I, V>::Nodo* nodoId = c.buscarNodo(id);
    //Si el nodo existe y es dependiente, pasa a independiente.
    if (nodoId != nullptr && nodoId->esDependiente) {
        typename ColecInterdep<I, V>::Nodo* sup = c.buscarNodo(nodoId->identSup);
        if (sup != nullptr) {
            sup->numDepend--;
        }
        nodoId->esDependiente = false;
    }
}

/*
 * Pre: c debe estar inicializada.
 * Post: Actualiza el val del nodo con id a nuevo si existe.
 * Devuelve true si se actualizó, false si no existe.
 *
 * Complejidad: O(h) - una sola búsqueda.
 */
template<typename I, typename V>
bool actualizarVal(ColecInterdep<I, V>& c, const I& id, const V& nuevo) {
    typename ColecInterdep<I, V>::Nodo* n = c.buscarNodo(id);
    if (n != nullptr) {
        n->val = nuevo;
        return true;
    }
    return false;
}

/*
 * Pre: c debe estar inicializada y existe un nodo con id.
 * Post: Devuelve el val del nodo con id.
 * Devuelve true si existe y se obtuvo, false si no existe.
 * 
 * Complejidad: O(h) - una sola búsqueda.
 */
template<typename I, typename V>
bool obtenerVal(const ColecInterdep<I, V>& c, const I& id, V& resultado) {
    const typename ColecInterdep<I, V>::Nodo* n = c.buscarNodo(id);
    if (n != nullptr) {
        resultado = n->val;
        return true;
    }
    return false;
}

/*
 * Pre: c debe estar inicializada y el nodo con id es dependiente.
 * Post: Devuelve el identSup del nodo con id.
 * Devuelve true si existe y se obtuvo, false si no existe.
 * 
 * Complejidad: O(h) - una búsqueda.
 */
template<typename I, typename V>
bool obtenerSupervisor(const ColecInterdep<I, V>& c, const I& id, I& resultado) {
    const typename ColecInterdep<I, V>::Nodo* n = c.buscarNodo(id);
    if (n != nullptr && n->esDependiente) {
        resultado = n->identSup;
        return true;
    }
    return false;
}

/*
 * Pre: c debe estar inicializada.
 * Post: Devuelve el numDepend del nodo con id si existe.
 * Devuelve true si existe y se obtuvo, false si no existe.
 * 
 * Complejidad: O(h) - una búsqueda.
 */
template<typename I, typename V>
bool obtenerNumDependientes(const ColecInterdep<I, V>& c, const I& id, int& resultado) {
    const typename ColecInterdep<I, V>::Nodo* n = c.buscarNodo(id);
    if (n != nullptr) {
        resultado = n->numDepend;
        return true;
    }
    return false;
}

/*
 * Pre: c debe estar inicializada.
 * Post: Borra el nodo con id si existe y su numDepend es 0, actualizando el super si es necesario.
 * 
 * Complejidad: O(h) - una búsqueda + borrado.
 */
template<typename I, typename V>
void borrar(ColecInterdep<I, V>& c, const I& id) {
    typename ColecInterdep<I, V>::Nodo* n = c.buscarNodo(id);
    
    //Solo se borra si el nodo existe y si ningún otro elemento depende de él (numDepend == 0).
    if (n != nullptr && n->numDepend == 0) {
        //Si el nodo a borrar dependía de otro (tenía supervisor), hay que actualizar el contador de ese supervisor.
        if (n->esDependiente) {
            typename ColecInterdep<I, V>::Nodo* sup = c.buscarNodo(n->identSup);
            if (sup != nullptr) {
                sup->numDepend--;
            }
        }
        ColecInterdep<I, V>::borrarNodoRec(c.raiz, id);
        c.numElementos--;
    }
}

/*
 * Pre: c debe estar inicializada.
 * Post: Borra todos los nodos de la colección y la deja vacía.
 * 
 * Complejidad: O(n) donde n es el número de elementos.
 */
template<typename I, typename V>
void borrarTodo(ColecInterdep<I, V>& c) {
    ColecInterdep<I, V>::liberarArbol(c.raiz);
    liberar(c.iterador);
    c.numElementos = 0;
}

// --- Operaciones del Iterador ---

/*
 * Pre: c debe estar inicializada.
 * Post: Inicializa el iterador al primer nodo.
 * 
 * Complejidad: O(h) donde h es la altura del árbol.
 */
template<typename I, typename V>
void iniciarIterador(ColecInterdep<I, V>& c) {
    liberar(c.iterador);
    if (c.raiz != nullptr) {
        ColecInterdep<I, V>::apilarRamaIzquierda(c.iterador, c.raiz);
    }
}

/*
 * Pre: c debe estar inicializada e iterador iniciado.
 * Post: Devuelve verdad si hay un siguiente nodo.
 * 
 * Complejidad: O(1).
 */
template<typename I, typename V>
bool existeSiguiente(const ColecInterdep<I, V>& c) {
    return !esVacia(c.iterador);
}

/*
 * Pre: c debe estar inicializada, iterador iniciado y existe siguiente.
 * Post: Devuelve el ident del nodo actual.
 * Devuelve true si hay elemento, false si el iterador está vacío.
 * 
 * Complejidad: O(1).
 */
template<typename I, typename V>
bool siguienteIdent(const ColecInterdep<I, V>& c, I& resultado) {
    typename ColecInterdep<I, V>::Nodo* n = ColecInterdep<I, V>::obtenerCima(c);
    if (n != nullptr) {
        resultado = n->ident;
        return true;
    }
    return false;
}

/*
 * Pre: c debe estar inicializada, iterador iniciado y existe siguiente.
 * Post: Devuelve el val del nodo actual.
 * Devuelve true si hay elemento, false si el iterador está vacío.
 * 
 * Complejidad: O(1).
 */
template<typename I, typename V>
bool siguienteVal(const ColecInterdep<I, V>& c, V& resultado) {
    typename ColecInterdep<I, V>::Nodo* n = ColecInterdep<I, V>::obtenerCima(c);
    if (n != nullptr) {
        resultado = n->val;
        return true;
    }
    return false;
}

/*
 * Pre: c debe estar inicializada, iterador iniciado y existe siguiente.
 * Post: Devuelve true si el nodo actual es dependiente, false si no.
 * 
 * Complejidad: O(1).
 */
template<typename I, typename V>
bool siguienteDependiente(const ColecInterdep<I, V>& c) {
    typename ColecInterdep<I, V>::Nodo* n = ColecInterdep<I, V>::obtenerCima(c);
    return (n != nullptr && n->esDependiente);
}

/*
 * Pre: c debe estar inicializada, iterador iniciado, existe siguiente y es dependiente.
 * Post: Devuelve el identSup del nodo actual.
 * Devuelve verdad si el nodo actual tiene un nodo supervisor, false si no.
 * 
 * Complejidad: O(1).
 */
template<typename I, typename V>
bool siguienteSuperior(const ColecInterdep<I, V>& c, I& resultado) {
    typename ColecInterdep<I, V>::Nodo* n = ColecInterdep<I, V>::obtenerCima(c);
    if (n != nullptr && n->esDependiente) { 
        resultado = n->identSup;
        return true;
    }
    return false;
}

/*
 * Pre: c debe estar inicializada, iterador iniciado y existe siguiente.
 * Post: Devuelve el numDepend del nodo actual.
 * 
 * Complejidad: O(1).
 */
template<typename I, typename V>
bool siguienteNumDependientes(const ColecInterdep<I, V>& c, int& resultado) {
    typename ColecInterdep<I, V>::Nodo* n = ColecInterdep<I, V>::obtenerCima(c);
    if (n != nullptr) {
        resultado = n->numDepend;
        return true;
    }
    return false;
}

/*
 * Pre:  c debe estar inicializada, iterador iniciado y existe siguiente.
 * Post: Avanza el iterador al siguiente nodo.
 * 
 * Complejidad: O(1) por paso.
 */
template<typename I, typename V>
void avanza(ColecInterdep<I, V>& c) {
    if (existeSiguiente(c)) {
        typename ColecInterdep<I, V>::Nodo* actual = ColecInterdep<I, V>::obtenerCima(c);
        desapilar(c.iterador);
        
        if (actual->der != nullptr) {
            ColecInterdep<I, V>::apilarRamaIzquierda(c.iterador, actual->der);
        }
    }
}

#endif