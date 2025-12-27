/* 
* Autores: 
*    - Daniel Jal Burguete, 815450 
*    - Fernado Pastor Peralta, 897113
*/
#ifndef COLEC_INTERDEP_HPP
#define COLEC_INTERDEP_HPP

#include <string>

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
template<typename I, typename V> ColecInterdep<I, V> crear();

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
template<typename I, typename V> void actualizarVal(ColecInterdep<I, V>& c, const I& id, const V& nuevo);

/* Si en c hay algún elemento con ident igual a id, sea (id,v,-,num) o sea (id,v,sup,num), devuelve
    el dato v que forma parte del elemento con dicho id.
    Parcial: la operación no está definida si no existe?(id,c).*/
template<typename I, typename V> V obtenerVal(const ColecInterdep<I, V>& c, const I& id);

/* Si existeDependiente?(id,c), sea su forma (id, v, sup, NumDep), devuelve el dato sup que forma
   parte del elemento con dicho id.
   Parcial: la operación no está definida si no existeDependiente?(id,c).*/
template<typename I, typename V> I obtenerSupervisor(const ColecInterdep<I, V>& c, const I& id);

/* Si existe?(id,c), sea su forma (id, v, -, NumDep) o (id, v, sup, NumDep), devuelve el dato NumDep
   que forma parte del elemento con dicho id. 
   Parcial: la operación no está definida si no existe?(id,c).*/
template<typename I, typename V> int obtenerNumDependientes(const ColecInterdep<I, V>& c, const I& id);

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
template<typename I, typename V> I siguienteIdent(const ColecInterdep<I, V>& c);

/* Devuelve el val del siguiente elemento a visitar con el iterador de la colección c, que será el
   elemento no visitado con ident anterior a los de todos los demás aún no visitados.
   Parcial: la operación no está definida si no quedan elementos por visitar (no existeSiguiente?(c))*/
template<typename I, typename V> V siguienteVal(const ColecInterdep<I, V>& c);

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
template<typename I, typename V> I siguienteSuperior(const ColecInterdep<I, V>& c);

/*Devuelve el NumDep del siguiente elemento a visitar con el iterador de la colección c, qu
  será el elemento no visitado con ident anterior a los de todos los demás aún no visitados.
  Parcial: la operación no está definida si no quedan elementos por visitar (no existeSiguiente?(c))*/
template<typename I, typename V> int siguienteNumDependientes(const ColecInterdep<I, V>& c);

/* Avanza el iterador de la colección c para que se pueda visitar otro elemento.
   Parcial: la operación no está definida si no quedan elementos por visitar (no existeSiguiente?(c))*/
template<typename I, typename V> void avanza(ColecInterdep<I, V>& c);

//FIN DE LAS OPERACIONES DEL TAD

//DECLARACIÓN DEL TAD  coleccionesInterdependientes:
template<typename I, typename V>
struct ColecInterdep {
    /* Declaración de la representación interna del tipo:
    *  Tiene dos componentes principales:
    *    1. puntero al primer nodo de la lista enlazada ordenada que almacena los elementos de la colección.
    *    2. puntero al nodo actual del iterador.
    *  Cada Nodo está formado por:
    *    1. ident: identificador del elemento.
    *    2. val: valor asociado al elemento.
    *    3. esDependiente: booleano que indica si el elemento es dependiente o independiente.
    *    4. identSup: identificador del elemento del que depende (si es dependiente).
    *    5. numDepend: número de elementos que dependen directamente de este elemento.
    *    6. siguiente: puntero al siguiente nodo en la lista enlazada.
    */
    private:
        struct Nodo {
            I ident;
            V val;
            bool esDependiente;
            I identSup;
            int numDepend;
            Nodo* siguiente;
        };

        Nodo* primero;
        Nodo* iterador;

        template<typename I2, typename V2>
        friend typename ColecInterdep<I2, V2>::Nodo* buscarNodo(const ColecInterdep<I2, V2>&, const I2&);
    public:
        friend ColecInterdep<I, V> crear<I, V>();
        friend int tamaño<I, V>(const ColecInterdep<I, V>& c);
        friend bool esVacia<I, V>(const ColecInterdep<I, V>& c);
        friend bool existe<I, V>(const ColecInterdep<I, V>& c, const I& id);
        friend bool existeDependiente<I, V>(const ColecInterdep<I, V>& c, const I& id);
        friend bool existeIndependiente<I, V>(const ColecInterdep<I, V>& c, const I& id);
        friend void añadirIndependiente<I, V>(ColecInterdep<I, V>& c, const I& id, const V& v);
        friend void añadirDependiente<I, V>(ColecInterdep<I, V>& c, const I& id, const V& v, const I& super);
        friend void hacerDependiente<I, V>(ColecInterdep<I, V>& c, const I& id, const I& super);
        friend void hacerIndependiente<I, V>(ColecInterdep<I, V>& c, const I& id);
        friend void actualizarVal<I, V>(ColecInterdep<I, V>& c, const I& id, const V& nuevo);
        friend V obtenerVal<I, V>(const ColecInterdep<I, V>& c, const I& id);
        friend I obtenerSupervisor<I, V>(const ColecInterdep<I, V>& c, const I& id);
        friend int obtenerNumDependientes<I, V>(const ColecInterdep<I, V>& c, const I& id);
        friend void borrar<I, V>(ColecInterdep<I, V>& c, const I& id);
        friend void borrarTodo<I, V>(ColecInterdep<I, V>& c);
        //OPERACIONES DEL ITERADOR
        friend void iniciarIterador<I, V>(ColecInterdep<I, V>& c);
        friend bool existeSiguiente<I, V>(const ColecInterdep<I, V>& c);
        friend I siguienteIdent<I, V>(const ColecInterdep<I, V>& c);
        friend V siguienteVal<I, V>(const ColecInterdep<I, V>& c);
        friend bool siguienteDependiente<I, V>(const ColecInterdep<I, V>& c);
        friend I siguienteSuperior<I, V>(const ColecInterdep<I, V>& c);
        friend int siguienteNumDependientes<I, V>(const ColecInterdep<I, V>& c);
        friend void avanza<I, V>(ColecInterdep<I, V>& c);
};

// --- FIN DE LA PARTE PÚBLICA o INTERFAZ ---



// IMPLEMENTACIÓN DE LAS FUNCIONES DEL TAD coleccionesInterdependientes:

// --- Operaciones auxiliares (no parte de la especificación pública) ---

/* 
 * Pre:  c debe estar inicializada.
 * Post: Devuelve un puntero al nodo con ident igual a id, o nullptr si no existe.
 */
template<typename I, typename V>
typename ColecInterdep<I, V>::Nodo* buscarNodo(const ColecInterdep<I, V>& c, const I& id) {
    typename ColecInterdep<I, V>::Nodo* aux = c.primero;
    while (aux != nullptr && !igual(aux->ident, id)) {
        aux = aux->siguiente;
    }
    return aux;
}

// --- OPERACIONES DEL TAD COLECINTERDEP ---

/* 
 * Pre:  --- 
 * Post: Devuelve una colección inicializada y vacía.
 */
template<typename I, typename V>
ColecInterdep<I, V> crear() {
    ColecInterdep<I, V> c;
    c.primero = nullptr;
    c.iterador = nullptr;
    return c;
}

/* 
 * Pre:  c debe estar inicializada.
 * Post: Devuelve el número de elementos en c.
 */
template<typename I, typename V>
int tamaño(const ColecInterdep<I, V>& c) {
    int count = 0;
    typename ColecInterdep<I, V>::Nodo* aux = c.primero;
    while (aux != nullptr) {
        count++;
        aux = aux->siguiente;
    }
    return count;
}

/* 
 * Pre:  c debe estar inicializada.
 * Post: Devuelve true si y solo si c no contiene ningún elemento.
 */
template<typename I, typename V>
bool esVacia(const ColecInterdep<I, V>& c) {
    return c.primero == nullptr;
}

/* 
 * Pre:  c debe estar inicializada.
 * Post: Devuelve verdad si existe un nodo con ident igual a id.
 */
template<typename I, typename V>
bool existe(const ColecInterdep<I, V>& c, const I& id) {
    return buscarNodo(c, id) != nullptr;
}

/* 
 * Pre:  c debe estar inicializada.
 * Post: Devuelve verdad si existe un nodo con ident igual a id y es dependiente.
 */
template<typename I, typename V>
bool existeDependiente(const ColecInterdep<I, V>& c, const I& id) {
    typename ColecInterdep<I, V>::Nodo* nodo = buscarNodo(c, id);
    return nodo != nullptr && nodo->esDependiente;
}

/* 
 * Pre:  c debe estar inicializada.
 * Post: Devuelve verdad si existe un nodo con ident igual a id y es independiente.
 */
template<typename I, typename V>
bool existeIndependiente(const ColecInterdep<I, V>& c, const I& id) {
    typename ColecInterdep<I, V>::Nodo* nodo = buscarNodo(c, id);
    return nodo != nullptr && !nodo->esDependiente;
}

/* 
 * Pre:  c debe estar inicializada.
 * Post: Añade un nodo independiente si no existe id, insertándolo en orden.
 */
template<typename I, typename V>
void añadirIndependiente(ColecInterdep<I, V>& c, const I& id, const V& v) {
    if (!existe(c, id)) {
        typename ColecInterdep<I, V>::Nodo* nuevo = new typename ColecInterdep<I, V>::Nodo;
        nuevo->ident = id;
        nuevo->val = v;
        nuevo->numDepend = 0;
        nuevo->siguiente = nullptr;
        nuevo->esDependiente = false;
        
        if (c.primero == nullptr || anterior(id, c.primero->ident)) {
            nuevo->siguiente = c.primero;
            c.primero = nuevo;
        } else {
           typename ColecInterdep<I, V>::Nodo* act = c.primero;
            while (act->siguiente != nullptr && anterior(act->siguiente->ident, id)) {
                act = act->siguiente;
            }
            nuevo->siguiente = act->siguiente;
            act->siguiente = nuevo;
        }
    }
}

/* 
 * Pre:  c debe estar inicializada.
 * Post: Hace dependiente el nodo con id a super, actualizando numDepend si es necesario.
 */
template<typename I, typename V>
void añadirDependiente(ColecInterdep<I, V>& c, const I& id, const V& v, const I& super) {
    typename ColecInterdep<I, V>::Nodo* nodoSuper = buscarNodo(c, super);
    if (!existe(c, id) && nodoSuper != nullptr) {
        nodoSuper->numDepend++;
        
        typename ColecInterdep<I, V>::Nodo* nuevo = new typename ColecInterdep<I, V>::Nodo;
        nuevo->ident = id;
        nuevo->val = v;
        nuevo->numDepend = 0;
        nuevo->siguiente = nullptr;
        nuevo->esDependiente = true;
        nuevo->identSup = super;

        if (c.primero == nullptr || anterior(id, c.primero->ident)) {
            nuevo->siguiente = c.primero;
            c.primero = nuevo;
        } else {
            typename ColecInterdep<I, V>::Nodo* act = c.primero;
            while (act->siguiente != nullptr && anterior(act->siguiente->ident, id)) {
                act = act->siguiente;
            }
            nuevo->siguiente = act->siguiente;
            act->siguiente = nuevo;
        }
    }
}

/* 
 * Pre:  c debe estar inicializada.
 * Post: Hace dependiente el nodo con id a super, actualizando numDepend si es necesario.
 */
template<typename I, typename V>
void hacerDependiente(ColecInterdep<I, V>& c, const I& id, const I& super) {
    if (igual(id, super)) return;

    typename ColecInterdep<I, V>::Nodo* nodoId = buscarNodo(c, id);
    typename ColecInterdep<I, V>::Nodo* nodoSuper = buscarNodo(c, super);

    if (nodoId != nullptr && nodoSuper != nullptr) {
        if (nodoId->esDependiente) { // Ya era dependiente
            typename ColecInterdep<I, V>::Nodo* nodoSuperAnt = buscarNodo(c, nodoId->identSup);
            if(nodoSuperAnt != nullptr) {
                nodoSuperAnt->numDepend--;
            }
        }
        nodoSuper->numDepend++;
        nodoId->esDependiente = true;
        nodoId->identSup = super;
    }
}

/* 
 * Pre:  c debe estar inicializada.
 * Post: Hace independiente el nodo con id si es dependiente, decrementando numDepend de su super.
 */
template<typename I, typename V>
void hacerIndependiente(ColecInterdep<I, V>& c, const I& id) {
    typename ColecInterdep<I, V>::Nodo* nodoId = buscarNodo(c, id);
    if (nodoId != nullptr && nodoId->esDependiente) {
        typename ColecInterdep<I, V>::Nodo* nodoSuperAnt = buscarNodo(c, nodoId->identSup);
        if (nodoSuperAnt != nullptr) {
            nodoSuperAnt->numDepend--;
        }
        nodoId->esDependiente = false;
    }
}

/* 
 * Pre:  c debe estar inicializada.
 * Post: Actualiza el val del nodo con id a nuevo si existe.
 */
template<typename I, typename V>
void actualizarVal(ColecInterdep<I, V>& c, const I& id, const V& nuevo) {
    typename ColecInterdep<I, V>::Nodo* nodo = buscarNodo(c, id);
    if (nodo != nullptr) {
        nodo->val = nuevo;
    }
}

/* 
 * Pre:  c debe estar inicializada y existe un nodo con id.
 * Post: Devuelve el val del nodo con id.
 */
template<typename I, typename V>
V obtenerVal(const ColecInterdep<I, V>& c, const I& id) {
    typename ColecInterdep<I, V>::Nodo* nodo = buscarNodo(c, id);
    return nodo->val;
}

/* 
 * Pre:  c debe estar inicializada y el nodo con id es dependiente.
 * Post: Devuelve el identSup del nodo con id.
 */
template<typename I, typename V>
I obtenerSupervisor(const ColecInterdep<I, V>& c, const I& id) {
    typename ColecInterdep<I, V>::Nodo* nodo = buscarNodo(c, id);
    return nodo->identSup;
}

/* 
 * Pre:  c debe estar inicializada.
 * Post: Devuelve el numDepend del nodo con id si existe.
 */
template<typename I, typename V>
int obtenerNumDependientes(const ColecInterdep<I, V>& c, const I& id) {
    typename ColecInterdep<I, V>::Nodo* nodo = buscarNodo(c, id);
    return nodo->numDepend;
}

/* 
 * Pre:  c debe estar inicializada.
 * Post: Borra el nodo con id si existe y su numDepend es 0, actualizando el super si es necesario.
 */
template<typename I, typename V>
void borrar(ColecInterdep<I, V>& c, const I& id) {
    typename ColecInterdep<I, V>::Nodo* actual = c.primero;
    typename ColecInterdep<I, V>::Nodo* previo = nullptr;

    while(actual != nullptr && !igual(actual->ident, id)) {
        previo = actual;
        actual = actual->siguiente;
    }

    if (actual != nullptr && actual->numDepend == 0) {
        if (actual->esDependiente) {
            typename ColecInterdep<I, V>::Nodo* nodoSuper = buscarNodo(c, actual->identSup);
            if (nodoSuper != nullptr) {
                nodoSuper->numDepend--;
            }
        }

        if (previo == nullptr) { // Borrar el primero
            c.primero = actual->siguiente;
        } else {
            previo->siguiente = actual->siguiente;
        }
        delete actual;
    }
}

/*
 * Pre: c debe estar inicializada.
 * Post: Borra todos los nodos de la colección y la deja vacía.
 */
template<typename I, typename V>
void borrarTodo(ColecInterdep<I, V>& c) {
    typename ColecInterdep<I, V>::Nodo* actual = c.primero;
    while(actual != nullptr){
        typename ColecInterdep<I, V>::Nodo* a_borrar = actual;
        actual = actual->siguiente;
        delete a_borrar;
    }
    c.primero = nullptr;
}

// --- Operaciones del Iterador ---

/* 
 * Pre:  c debe estar inicializada.
 * Post: Inicializa el iterador al primer nodo.
 */
template<typename I, typename V>
void iniciarIterador(ColecInterdep<I, V>& c) {
    c.iterador = c.primero;
}

/* 
 * Pre:  c debe estar inicializada e iterador iniciado.
 * Post: Devuelve verdad si hay un siguiente nodo.
 */
template<typename I, typename V>
bool existeSiguiente(const ColecInterdep<I, V>& c) {
    return c.iterador != nullptr;
}

/* 
 * Pre:  c debe estar inicializada, iterador iniciado y existe siguiente.
 * Post: Devuelve el ident del nodo actual.
 */
template<typename I, typename V>
I siguienteIdent(const ColecInterdep<I, V>& c) {
    return c.iterador->ident;
}

/* 
 * Pre:  c debe estar inicializada, iterador iniciado y existe siguiente.
 * Post: Devuelve el val del nodo actual.
 */
template<typename I, typename V>
V siguienteVal(const ColecInterdep<I, V>& c) {
    return c.iterador->val;
}

/* 
 * Pre:  c debe estar inicializada, iterador iniciado y existe siguiente.
 * Post: Devuelve verdad si el nodo actual es dependiente.
 */
template<typename I, typename V>
bool siguienteDependiente(const ColecInterdep<I, V>& c) {
    return c.iterador->esDependiente;
}

/* 
 * Pre:  c debe estar inicializada, iterador iniciado, existe siguiente y es dependiente.
 * Post: Devuelve el identSup del nodo actual.
 */
template<typename I, typename V>
I siguienteSuperior(const ColecInterdep<I, V>& c) {
    return c.iterador->identSup;
}

/* 
 * Pre:  c debe estar inicializada, iterador iniciado y existe siguiente.
 * Post: Devuelve el numDepend del nodo actual.
 */
template<typename I, typename V>
int siguienteNumDependientes(const ColecInterdep<I, V>& c) {
    return c.iterador->numDepend;
}

/* 
 * Pre:  c debe estar inicializada, iterador iniciado y existe siguiente.
 * Post: Avanza el iterador al siguiente nodo.
 */
template<typename I, typename V>
void avanza(ColecInterdep<I, V>& c) {
    if (existeSiguiente(c)) {
        c.iterador = c.iterador->siguiente;
    }
}

#endif