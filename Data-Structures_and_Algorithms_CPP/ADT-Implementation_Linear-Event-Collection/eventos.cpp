/* 
* Autores: 
*    - Daniel Jal Burguete, 815450 
*    - Fernado Pastor Peralta, 897113
*/

#include "eventos.hpp"

// IMPLEMENTACON DE LAS OPERACIONES DEL TAD Eventos:

/*
 * Pre:  --- 
 * Post: Devuelve un evento con descripción descrip y prioridad prio.
 */
Evento crearEvento(const std::string& descrip, int prio) {
    Evento e;
    e.descripcion = descrip;
    e.prioridad = prio;
    return e;
}

/* 
 * Pre:  e debe estar inicializado.
 * Post: Devuelve la descripción de e.
 */
std::string descripcion(const Evento& e) {
    return e.descripcion;
}

/* 
 * Pre:  e debe estar inicializado.
 * Post: Devuelve un nuevo evento con descripción cambiada a nueva.
 */
Evento cambiarDescripcion(const Evento& e, const std::string& nueva) {
    Evento nuevoEvento = e;
    nuevoEvento.descripcion = nueva;
    return nuevoEvento;
}

/* 
 * Pre:  e debe estar inicializado.
 * Post: Devuelve la prioridad de e.
 */
int suPrioridad(const Evento& e) {
    return e.prioridad;
}

/* 
 * Pre:  e debe estar inicializado.
 * Post: Devuelve un nuevo evento con prioridad cambiada a pri.
 */
Evento cambiarPrioridad(const Evento& e, int pri) {
    Evento nuevoEvento = e;
    nuevoEvento.prioridad = pri;
    return nuevoEvento;
}
