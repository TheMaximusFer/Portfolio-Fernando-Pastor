// Autores: Hugo López Sola - 875455 y Fernando Pastor Peralta - 897113

#ifndef COLA_HPP
#define COLA_HPP
#include <iostream>

// --- INICIO DE LA PARTE PÚBLICA O INTERFAZ ---

/*
* Los valores del TAD cola representan secuencias de elementos con acceso FIFO (first
* in, first out), esto es, el primer elemento en ser añadido (entrar) será el primero en ser borrado
* (salir). Para recorrer los elementos de la secuencia, ofrece las operaciones de un iterador,
* definido para recorrer los elementos en sentido de primero a último.
* Declaración adelantada del TAD Cola:
*/
template <typename T>
struct Cola;

// Interfaz del TAD genérico Cola. Pre-declaraciones:

// Devuelve una cola vacía, sin elementos.
template <typename T>
Cola<T> crear();

// Devuelve la cola igual a la resultante de añadir e a c como último elemento.
template <typename T>
void encolar(Cola<T> &c, const T &e);

// Devuelve verdad si y solo si c no tiene elementos.
template <typename T>
bool esVacia(const Cola<T> &c);

/*
* Si c no está vacía, devuelve false y el parámetro "primero" guardará el primer 
* elemento encolado de los que hay en c. Si está vacía, devuelve true y "primero"
* no se actualiza.
*/
template <typename T>
bool primero(const Cola<T> &c, T &primero);

/*
* Si c es no vacía, devuelve la cola igual a la resultante de eliminar de c el 
* primer elemento que fue encolado. En caso contrario, devuelve una cola igual a c.
*/
template <typename T>
void desencolar(Cola<T> &c);

// Devuelve el nº de elementos de c.
template <typename T>
int longitud(const Cola<T> &c);

/*
* Prepara el iterador y su cursor para que el siguiente elemento a visitar sea el 
* primero de la cola c (situación de no haber visitado ningún elemento).
*/
template <typename T>
void iniciarIterador(Cola<T> &c);

// Devuelve falso si ya se ha visitado el último elemento, devuelve verdad en caso contrario.
template <typename T>
bool existeSiguiente(const Cola<T> &c);

/*
* Si existe un siguiente elemento, devuelve false y en "e" se devolverá el siguiente elemento de c.
* Por el contrario, se devolverá true y "e" no se actualiza.
*/
template <typename T>
bool siguiente(const Cola<T> &c, T &e);

/*
* Si existe un siguiente elemento, devuelve false y "c" será devuelta con la cola resultante de avanzar
* el cursor en "c". Por el contrario, se devolverá true y "c" no se actualiza.
*/
template <typename T>
bool avanza(Cola<T> &c);

// --- FIN DE LA PARTE PÚBLICA o INTERFAZ ---

// Declaración:
template <typename T>
struct Cola
{
private: // Declaración interna del tipo:
    struct Nodo
    {
        T dato;
        Nodo *siguiente;
    };
    Nodo *prim;
    Nodo *ultimo;
    int n;
    Nodo *cursor;

public:
    friend Cola<T> crear<T>();
    friend void encolar<T>(Cola<T> &c, const T &e);
    friend bool esVacia<T>(const Cola<T> &c);
    friend bool primero<T>(const Cola<T> &c, T &primero);
    friend void desencolar<T>(Cola<T> &c);
    friend int longitud<T>(const Cola<T> &c);
    friend void iniciarIterador<T>(Cola<T> &c);
    friend bool existeSiguiente<T>(const Cola<T> &c);
    friend bool siguiente<T>(const Cola<T> &c, T &e);
    friend bool avanza<T>(Cola<T> &c);
};

// Implementacion de las funciones del TAD Cola:

// Devuelve una cola vacía, sin elementos.
template <typename T>
Cola<T> crear()
{
    Cola<T> c;
    c.prim = nullptr;
    c.ultimo = nullptr;
    c.n = 0;
    c.cursor = nullptr;
    return c;
}

// Devuelve la cola igual a la resultante de añadir e a c como último elemento.
template <typename T>
void encolar(Cola<T> &c, const T &e)
{
    typename Cola<T>::Nodo *nuevo = new typename Cola<T>::Nodo;
    nuevo->dato = e;
    nuevo->siguiente = nullptr;
    if (c.ultimo != nullptr)
    {
        c.ultimo->siguiente = nuevo;
    }
    c.ultimo = nuevo;
    if (c.prim == nullptr)
    {
        c.prim = nuevo;
    }
    c.n++;
}

// Devuelve verdad si y solo si c no tiene elementos.
template <typename T>
bool esVacia(const Cola<T> &c)
{
    return c.prim == nullptr;
}

/*
* Si c no está vacía, devuelve false y el parámetro "primero" guardará el primer 
* elemento encolado de los que hay en c. Si está vacía, devuelve true y "primero"
* no se actualiza.
*/
template <typename T>
bool primero(const Cola<T> &c, T &primero)
{
    if (c.prim == nullptr)
    {
        return false;
    }
    else
    {
        primero = c.prim->dato;
        return true;
    }
}

/*
* Si c es no vacía, devuelve la cola igual a la resultante de eliminar de c el 
* primer elemento que fue encolado. En caso contrario, devuelve una cola igual a c.
*/
template <typename T>
void desencolar(Cola<T> &c)
{
    if (c.prim != nullptr)
    {
        typename Cola<T>::Nodo *aBorrar = c.prim;
        c.prim = c.prim->siguiente;
        delete aBorrar;
        c.n--;
        if (c.prim == nullptr)
        {
            c.ultimo = nullptr;
        }
    }
}

// Devuelve el nº de elementos de c.
template <typename T>
int longitud(const Cola<T> &c)
{
    return c.n;
}

// Las cuatro operaciones siguientes conforman un iterador interno para la Cola:

/*
* Prepara el iterador y su cursor para que el siguiente elemento a visitar sea el 
* primero de la cola c (situación de no haber visitado ningún elemento).
*/
template <typename T>
void iniciarIterador(Cola<T> &c)
{
    c.cursor = c.prim;
}

// Devuelve falso si ya se ha visitado el último elemento, devuelve verdad en caso contrario.
template <typename T>
bool existeSiguiente(const Cola<T> &c)
{
    return c.cursor != nullptr;
}

/*
* Si existe un siguiente elemento, devuelve false y en "e" se devolverá el siguiente elemento de c.
* Por el contrario, se devolverá true y "e" no se actualiza.
*/
template <typename T>
bool siguiente(const Cola<T> &c, T &e)
{
    if (c.cursor == nullptr)
    {
        return false;
    }
    else
    {
        e = c.cursor->dato;
        return true;
    }
}

/*
* Si existe un siguiente elemento, devuelve false y "c" será devuelta con la cola resultante de avanzar
* el cursor en "c". Por el contrario, se devolverá true y "c" no se actualiza.
*/
template <typename T>
bool avanza(Cola<T> &c)
{
    if (c.cursor != nullptr)
    {
        c.cursor = c.cursor->siguiente;
        return true;
    }
    else
    {
        return false;
    }
}

#endif // COLA_HPP
