// Autor: Hugo López Sola - 875455 y Fernando Pastor Peralta - 897113

#include <iostream>
#include <string>

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
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devuelve false y devuelve
 * el dato v que forma parte del elemento con dicho id.
 * Si no, la función devolverá true y no se actualizará v.
 */
template <typename ident, typename val>
bool obtenerVal(const ident &id, val &v, const ColeccionMon<ident, val> &c);

/*
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devolverá false y devuelve
 * el dato na que forma parte del elemento con dicho id.
 * Si no, la función devolverá true y no se actualizará na.
 */
template <typename ident, typename val>
bool obtenerNumActualizaciones(const ident &id, int &na, const ColeccionMon<ident, val> &c);

/*
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devolverá false y devuelve
 * el dato nc que forma parte del elemento con dicho id.
 * Si no, la función devolverá true y no se actualizará nc.
 */
template <typename ident, typename val>
bool obtenerNumConsultas(const ident &id, int &nc, const ColeccionMon<ident, val> &c);

/*
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devolverá false y devuelve
 * una colección igual a la resultante de sustituir dicho elemento por (id,v,na,nc+1) en la colección c.
 * Si no, la función devolverá true y no se actualizará c.
 */
template <typename ident, typename val>
bool registrarConsulta(const ident &id, ColeccionMon<ident, val> &c);

/*
 * Si en c hay algún elemento con ident igual a id, devuelve una colección igual a la resultante de
 * eliminar ese elemento de la colección c. En caso contrario, devuelve una colección igual a c.
 */
template <typename ident, typename val>
bool borrar(const ident &id, ColeccionMon<ident, val> &c);

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
 * Si quedan elementos por visitar, la función devuelve false y devuelve el ident del siguiente elemento
 * a visitar con el iterador de la colección c, que será el elemento no visitando con ident anterior a los de
 * todos los demás aún no visitados.
 * Si no, la función devolverá true y no actualizará ident.
 */
template <typename ident, typename val>
bool siguienteIdent(const ColeccionMon<ident, val> &c, ident &id);

/*
 * Si quedan elementos por visitar, la función devuelve false y devuelve el val del siguiente elemento
 * a visitar con el iterador de la colección c, que será el elemento no visitando con ident anterior
 * a los de todos los demás aún no visitados.
 * Si no, la función devolverá true y no actualizará val.
 */
template <typename ident, typename val>
bool siguienteVal(const ColeccionMon<ident, val> &c, val &v);

/*
 * Si quedan elementos por visitar, la función devuelve false y devuelve el NumAct del siguiente elemento
 * a visitar con el iterador de la colección c, que será el elemento no visitando con ident anterior a
 * los de todos los demás aún no visitados.
 * Si no, la función devolverá true y no actualizará na.
 */
template <typename ident, typename val>
bool siguienteNumActualizaciones(const ColeccionMon<ident, val> &c, int &na);

/*
 * Si quedan elementos por visitar, la función devuelve false y devuelve el NumConsult del siguiente elemento
 * a visitar con el iterador de la colección c, que será el elemento no visitando con ident anterior a los de
 * todos los demás aún no visitados.
 * Si no, la función devolverá true y no actualizará nc.
 */
template <typename ident, typename val>
bool siguienteNumConsultas(const ColeccionMon<ident, val> &c, int &nc);
/*
 * Si quedan elementos por visitar, la función devuelve false y avanza el iterador de la colección c para que
 * se pueda visitar otro elemento.
 * Si no, la función devolverá true y no actualizará c.
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
        Nodo *siguiente;
    };

    Nodo *primero;
    Nodo *cursor;
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
    c.primero = nullptr;
    c.cursor = nullptr;
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
    return c.primero == nullptr;
}

/*
 * Si en c no hay ningún elemento con ident igual a id, devuelve una colección igual a la resultante
 * de anadir el elemento (id,v,1,0) a la colección c. En caso contrario, devuelve una colección igual
 * a c.
 */
/*El nuevo elemento será introducido según su indet numéricamente en orden ascendente*/
template <typename ident, typename val>
ColeccionMon<ident, val> anadir(ColeccionMon<ident, val> &c, const ident &id, const val &v)
{
    typename ColeccionMon<ident, val>::Nodo *nuevo = new typename ColeccionMon<ident, val>::Nodo;
    nuevo->id = id;
    nuevo->v = v;
    nuevo->NumAct = 1;
    nuevo->NumConsult = 0;

    if (c.primero == nullptr || c.primero->id > id)
    {
        nuevo->siguiente = c.primero; // En caso de que el nuevo elemento sea el primero, señalará a nullptr
                                      // En caso de que ya existiese un primer elemento con id mayor al nuevo,
                                      //   el nuevo elemento será el primero
        c.primero = nuevo;
    }
    else
    {
        typename ColeccionMon<ident, val>::Nodo *actual = c.primero;
        while (actual->siguiente != nullptr && actual->siguiente->id < id)
        {
            actual = actual->siguiente;
        }
        nuevo->siguiente = actual->siguiente;
        actual->siguiente = nuevo;
    }
    c.n++;
    return c;

    // typename ColeccionMon<ident, val>::Nodo *nuevo = new typename ColeccionMon<ident, val>::Nodo;
    // nuevo->id = id;
    // nuevo->v = v;
    // nuevo->NumAct = 1;
    // nuevo->NumConsult = 0;
    // nuevo->siguiente = c.primero;
    // c.primero = nuevo;
    // c.n++;
    // return c;
}

/*
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devuelve false y devuelve
 * una colección igual a la resultante de sustituir dicho elemento por (id,nuevo,na+1,nc) en la colección c.
 * Si no, la función devolverá true y no se actualizará c.
 */
template <typename ident, typename val>
bool actualizar(ColeccionMon<ident, val> &c, const ident &id, const val &nuevo)
{
    typename ColeccionMon<ident, val>::Nodo *actual = c.primero;
    while (actual != nullptr && actual->id != id)
    {
        actual = actual->siguiente;
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
    typename ColeccionMon<ident, val>::Nodo *actual = c.primero;
    while (actual != nullptr && actual->id != id)
    {
        actual = actual->siguiente;
    }
    return actual != nullptr;
}

/*
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devuelve false y devuelve
 * el dato v que forma parte del elemento con dicho id.
 * Si no, la función devolverá true y no se actualizará v.
 */
template <typename ident, typename val>
bool obtenerVal(const ident &id, val &v, const ColeccionMon<ident, val> &c)
{
    typename ColeccionMon<ident, val>::Nodo *actual = c.primero;
    while (actual != nullptr && actual->id != id)
    {
        actual = actual->siguiente;
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
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devolverá false y devuelve
 * el dato na que forma parte del elemento con dicho id.
 * Si no, la función devolverá true y no se actualizará na.
 */
template <typename ident, typename val>
bool obtenerNumActualizaciones(const ident &id, int &na, const ColeccionMon<ident, val> &c)
{
    typename ColeccionMon<ident, val>::Nodo *actual = c.primero;
    while (actual != nullptr && actual->id != id)
    {
        actual = actual->siguiente;
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
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devolverá false y devuelve
 * el dato nc que forma parte del elemento con dicho id.
 * Si no, la función devolverá true y no se actualizará nc.
 */
template <typename ident, typename val>
bool obtenerNumConsultas(const ident &id, int &nc, const ColeccionMon<ident, val> &c)
{
    typename ColeccionMon<ident, val>::Nodo *actual = c.primero;
    while (actual != nullptr && actual->id != id)
    {
        actual = actual->siguiente;
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
 * Si en c hay algún elemento con ident igual a id, sea (id,v,na,nc), la función devolverá false y devuelve
 * una colección igual a la resultante de sustituir dicho elemento por (id,v,na,nc+1) en la colección c.
 * Si no, la función devolverá true y no se actualizará c.
 */
template <typename ident, typename val>
bool registrarConsulta(const ident &id, ColeccionMon<ident, val> &c)
{
    typename ColeccionMon<ident, val>::Nodo *actual = c.primero;
    while (actual != nullptr && actual->id != id)
    {
        actual = actual->siguiente;
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
 * eliminar ese elemento de la colección c. En caso contrario, devuelve una colección igual a c.
 */
template <typename ident, typename val>
bool borrar(const ident &id, ColeccionMon<ident, val> &c)
{
    typename ColeccionMon<ident, val>::Nodo *actual = c.primero;
    typename ColeccionMon<ident, val>::Nodo *anterior = nullptr;
    while (actual != nullptr && actual->id != id)
    {
        anterior = actual;
        actual = actual->siguiente;
    }
    if (actual != nullptr)
    {
        if (anterior == nullptr)
        {
            c.primero = actual->siguiente;
        }
        else
        {
            anterior->siguiente = actual->siguiente;
        }
        delete actual;
        c.n--;
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * Inicializa el iterador para recorrer los elementos de la colección c, de forma que el siguiente
 * elemento a visitar sea el que tiene un ident anterior a los de todos los demás elementos de la
 * colección (situación de no haber visitado ningún elemento).
 */
template <typename ident, typename val>
void iniciarIterador(ColeccionMon<ident, val> &c)
{
    c.cursor = c.primero;
}

/*
 * Devuelve verdad si queda algún elemento por visitar con el iterador de la colección c, devuelve
 * falso si ya se ha visitado el último elemento.
 */
template <typename ident, typename val>
bool existeSiguiente(const ColeccionMon<ident, val> &c)
{
    return c.cursor != nullptr;
}

/*
 * Si quedan elementos por visitar, la función devuelve false y devuelve el ident del siguiente elemento
 * a visitar con el iterador de la colección c, que será el elemento no visitando con ident anterior a los de
 * todos los demás aún no visitados.
 * Si no, la función devolverá true y no actualizará ident.
 */
template <typename ident, typename val>
bool siguienteIdent(const ColeccionMon<ident, val> &c, ident &id)
{
    if (c.cursor != nullptr)
    {
        id = c.cursor->id;
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * Si quedan elementos por visitar, la función devuelve false y devuelve el val del siguiente elemento
 * a visitar con el iterador de la colección c, que será el elemento no visitando con ident anterior
 * a los de todos los demás aún no visitados.
 * Si no, la función devolverá true y no actualizará val.
 */
template <typename ident, typename val>
bool siguienteVal(const ColeccionMon<ident, val> &c, val &v)
{
    if (c.cursor != nullptr)
    {
        v = c.cursor->v;
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * Si quedan elementos por visitar, la función devuelve false y devuelve el NumAct del siguiente elemento
 * a visitar con el iterador de la colección c, que será el elemento no visitando con ident anterior a
 * los de todos los demás aún no visitados.
 * Si no, la función devolverá true y no actualizará na.
 */
template <typename ident, typename val>
bool siguienteNumActualizaciones(const ColeccionMon<ident, val> &c, int &na)
{
    if (c.cursor != nullptr)
    {
        na = c.cursor->NumAct;
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * Si quedan elementos por visitar, la función devuelve false y devuelve el NumConsult del siguiente elemento
 * a visitar con el iterador de la colección c, que será el elemento no visitando con ident anterior a los de
 * todos los demás aún no visitados.
 * Si no, la función devolverá true y no actualizará nc.
 */
template <typename ident, typename val>
bool siguienteNumConsultas(const ColeccionMon<ident, val> &c, int &nc)
{
    if (c.cursor != nullptr)
    {
        nc = c.cursor->NumConsult;
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * Si quedan elementos por visitar, la función devuelve false y avanza el iterador de la colección c para que
 * se pueda visitar otro elemento.
 * Si no, la función devolverá true y no actualizará c.
 */
template <typename ident, typename val>
bool avanza(ColeccionMon<ident, val> &c)
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
