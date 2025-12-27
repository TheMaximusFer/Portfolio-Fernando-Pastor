// Autores: Hugo López Sola - 875455 y Fernando Pastor Peralta - 897113

#include <iostream>
#include <string>
#include "pila_dinamica.hpp"

// --- INICIO DE LA PARTE PÚBLICA O INTERFAZ ---

/*
 * Los valores del TAD representan colecciones de elementos formados como tuplas
 * (ident, val, NumAct, NumConsult) y tales que en cada tupla: NumAct representará el número de
 * veces que se ha registrado un val para dicho ident desde que este pertenece a la colección, y
 * NumConsult representará el número de veces que se ha registrado una consulta del val para dicho
 * ident. Cuando un ident se añade a la colección, su NumAct será 1 y su NumConsult será 0. En la
 * colección no se permitirán elementos con ident iguales.
 * Declaración adelantada del TAD ColeccionMon:
 */
/* Se implementa un arbol binario con el que almacenar la colección*/
template <typename ident, typename val>
struct ColeccionMon;

// Interfaz del TAD genérico coleccionMon. Pre-declaraciones:

// Crea una colección vacía, sin elementos.
template <typename ident, typename val>
ColeccionMon<ident, val> crear();

// Devuelve el número de elementos que hay en la colección c.
template <typename ident, typename val>
int tamano(const ColeccionMon<ident, val> &c);

// Devuelve verdad si y solo si c no contiene ningún elemento.
template <typename ident, typename val>
bool esVacia(const ColeccionMon<ident, val> &c);

/*
 * Si en c no hay ningún elemento con ident igual a id, devuelve una colección igual a la resultante
 * de anadir el elemento (id,v,1,0) a la colección c. En caso contrario, devuelve una colección igual
 * a c.
 * El nuevo elemento será introducido según su indet numéricamente en orden ascendente
 */
template <typename ident, typename val>
ColeccionMon<ident, val> anadir(ColeccionMon<ident, val> &c, const ident &id, const val &v);

/*
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devuelve false y devuelve
 * una colección igual a la resultante de sustituir dicho elemento por (id,nuevo,na+1,nc) en la colección c.
 * Si no, la función devolverá true y no se actualizará c.
 */
template <typename ident, typename val>
bool actualizar(ColeccionMon<ident, val> &c, const ident &id, const val &nuevo);

// Devuelve verdad si y solo si en c hay algún elemento con ident igual a id.
template <typename ident, typename val>
bool esta(const ident &id, const ColeccionMon<ident, val> &c);

/*
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devuelve true y devuelve
 * el dato v que forma parte del elemento con dicho id.
 * Si no, la función devolverá false y no se actualizará v.
 */
template <typename ident, typename val>
bool obtenerVal(const ident &id, val &v, const ColeccionMon<ident, val> &c);

/*
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devolverá true y devuelve
 * el dato na que forma parte del elemento con dicho id.
 * Si no, la función devolverá false y no se actualizará na.
 */
template <typename ident, typename val>
bool obtenerNumActualizaciones(const ident &id, int &na, const ColeccionMon<ident, val> &c);

/*
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devolverá true y devuelve
 * el dato nc que forma parte del elemento con dicho id.
 * Si no, la función devolverá false y no se actualizará nc.
 */
template <typename ident, typename val>
bool obtenerNumConsultas(const ident &id, int &nc, const ColeccionMon<ident, val> &c);

/*
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devolverá true y devuelve
 * una colección igual a la resultante de sustituir dicho elemento por (id,v,na,nc+1) en la colección c.
 * Si no, la función devolverá false y no se actualizará c.
 */
template <typename ident, typename val>
bool registrarConsulta(const ident &id, ColeccionMon<ident, val> &c);

/*
 * Si en c hay algún elemento con ident igual a id, devuelve una colección igual a la resultante de
 * eliminar ese elemento de la colección c y la función devolverá true. En caso contrario, 
 * devuelve una colección igual a c y la función devuelve false.
 */
template <typename ident, typename val>
bool borrar(const ident &id, ColeccionMon<ident, val> &c);

//OPERACIONES ITERADOR

/*
 * Inicializa el iterador para recorrer los elementos de la colección c, de forma que el siguiente
 * elemento a visitar sea el que tiene un ident anterior a los de todos los demás elementos de la
 * colección (situación de no haber visitado ningún elemento).
 */
template <typename ident, typename val>
void iniciarIterador(ColeccionMon<ident, val> &c);

/*
 * Devuelve verdad si queda algún elemento por visitar con el iterador de la colección c, devuelve
 * falso si ya se ha visitado el último elemento.
 */
template <typename ident, typename val>
bool existeSiguiente(const ColeccionMon<ident, val> &c);

/*
 * Si quedan elementos por visitar, la función devuelve true y devuelve el ident del siguiente elemento
 * a visitar con el iterador de la colección c, que será el elemento no visitando con ident anterior a los de
 * todos los demás aún no visitados.
 * Si no, la función devolverá false y no actualizará ident.
 */
template <typename ident, typename val>
bool siguienteIdent(const ColeccionMon<ident, val> &c, ident &id);

/*
 * Si quedan elementos por visitar, la función devuelve true y devuelve el val del siguiente elemento
 * a visitar con el iterador de la colección c, que será el elemento no visitando con ident anterior
 * a los de todos los demás aún no visitados.
 * Si no, la función devolverá false y no actualizará val.
 */
template <typename ident, typename val>
bool siguienteVal(const ColeccionMon<ident, val> &c, val &v);

/*
 * Si quedan elementos por visitar, la función devuelve true y devuelve el NumAct del siguiente elemento
 * a visitar con el iterador de la colección c, que será el elemento no visitando con ident anterior a
 * los de todos los demás aún no visitados.
 * Si no, la función devolverá false y no actualizará na.
 */
template <typename ident, typename val>
bool siguienteNumActualizaciones(const ColeccionMon<ident, val> &c, int &na);

/*
 * Si quedan elementos por visitar, la función devuelve true y devuelve el NumConsult del siguiente elemento
 * a visitar con el iterador de la colección c, que será el elemento no visitando con ident anterior a los de
 * todos los demás aún no visitados.
 * Si no, la función devolverá false y no actualizará nc.
 */
template <typename ident, typename val>
bool siguienteNumConsultas(const ColeccionMon<ident, val> &c, int &nc);

/*
 * Si quedan elementos por visitar, la función devuelve true y avanza el iterador de la colección c para que
 * se pueda visitar otro elemento.
 * Si no, la función devolverá false y no actualizará c.
 */
template <typename ident, typename val>
bool avanza(ColeccionMon<ident, val> &c);

// --- FIN DE LA PARTE PÚBLICA o INTERFAZ ---

// Declaración:
template <typename ident, typename val>
struct ColeccionMon
{
private: // Declaración interna del tipo:
    struct Nodo
    {
        ident id;
        val v;
        int NumAct;
        int NumConsult;
        Nodo *hijoIzq = nullptr;
        Nodo *hijoDer = nullptr;
    };

    Nodo* raiz; // Raíz del árbol binario
    Pila<Nodo*> pilaIterador; // Pila para gestionar el iterador
    int n;

public:
    friend ColeccionMon<ident, val> crear<ident, val>();
    friend int tamano<ident, val>(const ColeccionMon<ident, val> &c);
    friend bool esVacia<ident, val>(const ColeccionMon<ident, val> &c);
    friend ColeccionMon<ident, val> anadir<ident, val>(ColeccionMon<ident, val> &c, const ident &id, const val &v);
    friend bool actualizar<ident, val>(ColeccionMon<ident, val> &c, const ident &id, const val &nuevo);
    friend bool esta<ident, val>(const ident &id, const ColeccionMon<ident, val> &c);
    friend bool obtenerVal<ident, val>(const ident &id, val &v, const ColeccionMon<ident, val> &c);
    friend bool obtenerNumActualizaciones<ident, val>(const ident &id, int &na, const ColeccionMon<ident, val> &c);
    friend bool obtenerNumConsultas<ident, val>(const ident &id, int &nc, const ColeccionMon<ident, val> &c);
    friend bool registrarConsulta<ident, val>(const ident &id, ColeccionMon<ident, val> &c);
    friend bool borrar<ident, val>(const ident &id, ColeccionMon<ident, val> &c);

    //OPERACIONES ITERADOR
    friend void iniciarIterador<ident, val>(ColeccionMon<ident, val> &c);
    friend bool existeSiguiente<ident, val>(const ColeccionMon<ident, val> &c);
    friend bool siguienteIdent<ident, val>(const ColeccionMon<ident, val> &c, ident &id);
    friend bool siguienteVal<ident, val>(const ColeccionMon<ident, val> &c, val &v);
    friend bool siguienteNumActualizaciones<ident, val>(const ColeccionMon<ident, val> &c, int &na);
    friend bool siguienteNumConsultas<ident, val>(const ColeccionMon<ident, val> &c, int &nc);
    friend bool avanza<ident, val>(ColeccionMon<ident, val> &c);
};

// Implementación de las funciones del TAD ColeccionMon:

// Crea una colección vacía, sin elementos.
template <typename ident, typename val>
ColeccionMon<ident, val> crear()
{
    ColeccionMon<ident, val> c;
    c.raiz = nullptr;
    c.n = 0;
    return c;
}

// Devuelve el número de elementos que hay en la colección c.
template <typename ident, typename val>
int tamano(const ColeccionMon<ident, val> &c)
{
    return c.n;
}

// Devuelve verdad si y solo si c no contiene ningún elemento.
template <typename ident, typename val>
bool esVacia(const ColeccionMon<ident, val> &c)
{
    return c.raiz == nullptr;
}

/*
 * Si en c no hay ningún elemento con ident igual a id, devuelve una colección igual a la resultante
 * de anadir el elemento (id,v,1,0) a la colección c. En caso contrario, devuelve una colección igual
 * a c.
 * El nuevo elemento será introducido según su indet numéricamente en orden ascendente
 */
template <typename ident, typename val>
ColeccionMon<ident, val> anadir(ColeccionMon<ident, val> &c, const ident &id, const val &v)
{
    typename ColeccionMon<ident, val>::Nodo *nuevo = new typename ColeccionMon<ident, val>::Nodo;
    nuevo->id = id;
    nuevo->v = v;
    nuevo->NumAct = 1;
    nuevo->NumConsult = 0;

    if (c.raiz == nullptr)
    {
        c.raiz = nuevo;
        c.n++;
        return c;
    }
    else
    {
        typename ColeccionMon<ident, val>::Nodo *actual = c.raiz;
        typename ColeccionMon<ident, val>::Nodo *anterior = nullptr;
        while (actual != nullptr && actual->id != id)
        {
            if (actual->id > id)
            {
                anterior = actual;
                actual = actual->hijoIzq;
            }
            else if (actual->id < id)
            {
                anterior = actual;
                actual = actual->hijoDer;
            }
        }
        if (anterior->id > id)
        {
            anterior->hijoIzq = nuevo;
        }
        else if (anterior->id < id)
        {
            anterior->hijoDer = nuevo;
        }
        else
        {
            return c;
        }
    }

    c.n++;
    return c;
}

/*
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devuelve false y devuelve
 * una colección igual a la resultante de sustituir dicho elemento por (id,nuevo,na+1,nc) en la colección c.
 * Si no, la función devolverá true y no se actualizará c.
 */
template <typename ident, typename val>
bool actualizar(ColeccionMon<ident, val> &c, const ident &id, const val &nuevo)
{
    typename ColeccionMon<ident, val>::Nodo *actual = c.raiz;
    while (actual != nullptr && actual->id != id)
    {
        if (actual->id > id)
        {
            actual = actual->hijoIzq;
        }
        else if (actual->id < id)
        {
            actual = actual->hijoDer;
        }
    }
    if (actual != nullptr)
    {
        actual->v = nuevo;
        actual->NumAct++;
        return true;
    }
    else
    {
        return false;
    }
}

// Devuelve verdad si y solo si en c hay algún elemento con ident igual a id.
template <typename ident, typename val>
bool esta(const ident &id, const ColeccionMon<ident, val> &c)
{
    typename ColeccionMon<ident, val>::Nodo *actual = c.raiz;
    while (actual != nullptr && actual->id != id)
    {
        if (actual->id > id)
        {
            actual = actual->hijoIzq;
        }
        else if (actual->id < id)
        {
            actual = actual->hijoDer;
        }
    }
    if (actual != nullptr)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devuelve true y devuelve
 * el dato v que forma parte del elemento con dicho id.
 * Si no, la función devolverá false y no se actualizará v.
 */
template <typename ident, typename val>
bool obtenerVal(const ident &id, val &v, const ColeccionMon<ident, val> &c)
{
    typename ColeccionMon<ident, val>::Nodo *actual = c.raiz;
    while (actual != nullptr && actual->id != id)
    {
        if (actual->id > id)
        {
            actual = actual->hijoIzq;
        }
        else if (actual->id < id)
        {
            actual = actual->hijoDer;
        }
    }
    if (actual != nullptr)
    {
        v = actual->v;
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devolverá true y devuelve
 * el dato na que forma parte del elemento con dicho id.
 * Si no, la función devolverá false y no se actualizará na.
 */
template <typename ident, typename val>
bool obtenerNumActualizaciones(const ident &id, int &na, const ColeccionMon<ident, val> &c)
{
    typename ColeccionMon<ident, val>::Nodo *actual = c.raiz;
    while (actual != nullptr && actual->id != id)
    {
        if (actual->id > id)
        {
            actual = actual->hijoIzq;
        }
        else if (actual->id < id)
        {
            actual = actual->hijoDer;
        }
    }
    if (actual != nullptr)
    {
        na = actual->NumAct;
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devolverá true y devuelve
 * el dato nc que forma parte del elemento con dicho id.
 * Si no, la función devolverá false y no se actualizará nc.
 */
template <typename ident, typename val>
bool obtenerNumConsultas(const ident &id, int &nc, const ColeccionMon<ident, val> &c)
{
    typename ColeccionMon<ident, val>::Nodo *actual = c.raiz;
    while (actual != nullptr && actual->id != id)
    {
        if (actual->id > id)
        {
            actual = actual->hijoIzq;
        }
        else if (actual->id < id)
        {
            actual = actual->hijoDer;
        }
    }
    if (actual != nullptr)
    {
        nc = actual->NumConsult;
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devolverá true y devuelve
 * una colección igual a la resultante de sustituir dicho elemento por (id,v,na,nc+1) en la colección c.
 * Si no, la función devolverá false y no se actualizará c.
 */
template <typename ident, typename val>
bool registrarConsulta(const ident &id, ColeccionMon<ident, val> &c)
{
    typename ColeccionMon<ident, val>::Nodo *actual = c.raiz;
    while (actual != nullptr && actual->id != id)
    {
        if (actual->id > id)
        {
            actual = actual->hijoIzq;
        }
        else if (actual->id < id)
        {
            actual = actual->hijoDer;
        }
    }
    if (actual != nullptr)
    {
        actual->NumConsult++;
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * Si en c hay algún elemento con ident igual a id, devuelve una colección igual a la resultante de
 * eliminar ese elemento de la colección c y la función devolverá true. En caso contrario, 
 * devuelve una colección igual a c y la función devuelve false.
 */
template <typename ident, typename val>
bool borrar(const ident &id, ColeccionMon<ident, val> &c)
{
    typename ColeccionMon<ident, val>::Nodo *actual = c.raiz;
    typename ColeccionMon<ident, val>::Nodo *anterior = nullptr;

    //Buscar el nodo a eliminar.
    while (actual != nullptr && actual->id != id)
    {
        anterior = actual;
        if (actual->id > id)
        {
            actual = actual->hijoIzq;
        }
        else
        {
            actual = actual->hijoDer;
        }
    }

    //Si el nodo no se encuentra.
    if (actual == nullptr)
    {
        return false;
    }

    //Nodo a eliminar encontrado.
    if (actual->hijoIzq == nullptr && actual->hijoDer == nullptr)
    {
        //Caso 1: El nodo no tiene hijos.
        if (anterior == nullptr) // Es la raíz
        {
            c.raiz = nullptr;
        }
        else if (anterior->hijoIzq == actual)
        {
            anterior->hijoIzq = nullptr;
        }
        else
        {
            anterior->hijoDer = nullptr;
        }
        delete actual;
    }
    else if (actual->hijoIzq == nullptr || actual->hijoDer == nullptr)
    {
        //Caso 2: El nodo tiene un solo hijo.
        typename ColeccionMon<ident, val>::Nodo *hijo = (actual->hijoIzq != nullptr) ? actual->hijoIzq : actual->hijoDer;
        if (anterior == nullptr) //Es la raíz.
        {
            c.raiz = hijo;
        }
        else if (anterior->hijoIzq == actual)
        {
            anterior->hijoIzq = hijo;
        }
        else
        {
            anterior->hijoDer = hijo;
        }
        delete actual;
    }
    else
    {
        //Caso 3: El nodo tiene dos hijos.
        typename ColeccionMon<ident, val>::Nodo *sustituto = actual->hijoDer;
        typename ColeccionMon<ident, val>::Nodo *padreSustituto = actual;

        //Buscar el sucesor inorden.
        while (sustituto->hijoIzq != nullptr)
        {
            padreSustituto = sustituto;
            sustituto = sustituto->hijoIzq;
        }

        //Reemplazar el nodo.
        actual->id = sustituto->id;
        actual->v = sustituto->v;
        actual->NumAct = sustituto->NumAct;
        actual->NumConsult = sustituto->NumConsult;

        //Eliminar el sucesor inorden.
        if (padreSustituto->hijoIzq == sustituto)
        {
            padreSustituto->hijoIzq = sustituto->hijoDer;
        }
        else
        {
            padreSustituto->hijoDer = sustituto->hijoDer;
        }
        delete sustituto;
    }

    c.n--;
    return true;
}

/*
 * Inicializa el iterador para recorrer los elementos de la colección c, de forma que el siguiente
 * elemento a visitar sea el que tiene un ident anterior a los de todos los demás elementos de la
 * colección (situación de no haber visitado ningún elemento).
 */
template <typename ident, typename val>
void iniciarIterador(ColeccionMon<ident, val> &c) 
{
    crearVacia(c.pilaIterador); //Inicializamos la pila vacía.
    typename ColeccionMon<ident, val>::Nodo *actual = c.raiz;
    while (actual != nullptr) 
    {
        apilar(c.pilaIterador, actual); //Apilamos los nodos del subárbol izquierdo.
        actual = actual->hijoIzq;
    }
}

/*
 * Devuelve verdad si queda algún elemento por visitar con el iterador de la colección c, devuelve
 * falso si ya se ha visitado el último elemento.
 */
template <typename ident, typename val>
bool existeSiguiente(const ColeccionMon<ident, val> &c) 
{
    return !esVacia(c.pilaIterador);
}

/*
 * Si quedan elementos por visitar, la función devuelve true y devuelve el ident del siguiente elemento
 * a visitar con el iterador de la colección c, que será el elemento no visitando con ident anterior a los de
 * todos los demás aún no visitados.
 * Si no, la función devolverá false y no actualizará ident.
 */
template <typename ident, typename val>
bool siguienteIdent(const ColeccionMon<ident, val> &c, ident &id) 
{
    typename ColeccionMon<ident, val>::Nodo *actual;
    bool error;
    cima(c.pilaIterador, actual, error);
    if (!error) 
    {
        id = actual->id;
        return true;
    }else return false;
}

/*
 * Si quedan elementos por visitar, la función devuelve true y devuelve el val del siguiente elemento
 * a visitar con el iterador de la colección c, que será el elemento no visitando con ident anterior
 * a los de todos los demás aún no visitados.
 * Si no, la función devolverá false y no actualizará val.
 */
template <typename ident, typename val>
bool siguienteVal(const ColeccionMon<ident, val> &c, val &v) 
{
    typename ColeccionMon<ident, val>::Nodo *actual;
    bool error;
    cima(c.pilaIterador, actual, error);
    if (!error) 
    {
        v = actual->v;
        return true;
    }else return false;
}

/*
 * Si quedan elementos por visitar, la función devuelve true y devuelve el NumAct del siguiente elemento
 * a visitar con el iterador de la colección c, que será el elemento no visitando con ident anterior a
 * los de todos los demás aún no visitados.
 * Si no, la función devolverá false y no actualizará na.
 */
template <typename ident, typename val>
bool siguienteNumActualizaciones(const ColeccionMon<ident, val> &c, int &na) 
{
    typename ColeccionMon<ident, val>::Nodo *actual;
    bool error;
    cima(c.pilaIterador, actual, error);
    if (!error) 
    {
        na = actual->numActualizaciones;
        return true;
    }else return false; 
}

/*
 * Si quedan elementos por visitar, la función devuelve true y devuelve el NumConsult del siguiente elemento
 * a visitar con el iterador de la colección c, que será el elemento no visitando con ident anterior a los de
 * todos los demás aún no visitados.
 * Si no, la función devolverá false y no actualizará nc.
 */
template <typename ident, typename val>
bool siguienteNumConsultas(const ColeccionMon<ident, val> &c, int &nc) 
{
    typename ColeccionMon<ident, val>::Nodo *actual;
    bool error;
    cima(c.pilaIterador, actual, error);
    if (!error) 
    {
        nc = actual->numConsultas;
        return true;
    }else return false;
}

/*
 * Si quedan elementos por visitar, la función devuelve true y avanza el iterador de la colección c para que
 * se pueda visitar otro elemento.
 * Si no, la función devolverá false y no actualizará c.
 */
template <typename ident, typename val>
bool avanza(ColeccionMon<ident, val> &c) {
    typename ColeccionMon<ident, val>::Nodo *actual;
    bool error;
    cima(c.pilaIterador, actual, error);
    if (!error) {
        desapilar(c.pilaIterador); //Eliminamos el nodo actual de la pila.

        if (actual->hijoDer != nullptr) {
            actual = actual->hijoDer;
            while (actual != nullptr) {
                apilar(c.pilaIterador, actual); //Apilamos los nodos del subárbol derecho.
                actual = actual->hijoIzq;
            }
        }
        return true;
    }else return false;
}
