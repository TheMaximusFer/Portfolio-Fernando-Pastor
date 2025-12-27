/* 
* Autores: 
*    - Daniel Jal Burguete, 815450 
*    - Fernado Pastor Peralta, 897113
*/

#ifndef EVENTO_HPP
#define EVENTO_HPP

#include <string>


// INTERFAZ DEL TAD Eventos. PRE-DECLARACIONES:

/*
* Los valores del TAD evento representarán tuplas formadas como: 
* (descripción, prioridad) siendo la descripción una cadena y 
* la prioridad un número natural.
*/
struct Evento;


// OPERACIONES DEL TAD Eventos:

// Devuelve un evento compuesto con descripción descrip y con prioridad prio.
Evento crearEvento(const std::string& descrip, int prio);

// Dado un evento e formado como (D,P) devuelve la cadena D, i.e. la descripción en el evento e.
std::string descripcion(const Evento& e);

// Dado un evento e formado como (D,P) devuelve un evento igual al compuesto como (nueva,P).
Evento cambiarDescripcion(const Evento& e, const std::string& nueva);

// Dado un evento e formado como (D,P), devuelve P, i.e. la prioridad en el evento e.
int suPrioridad(const Evento& e);

// Dado un evento e formado como (D,P) y un natural pri, devuelve un evento igual al compuesto como (D,pri).
Evento cambiarPrioridad(const Evento& e, int pri);

//FIN DE LAS OPERACIONES DEL TAD

//DECLARACIÓN DEL TAD Evento:
struct Evento {
    private:
        /* Declaración de la representación interna del tipo:
        *  Tiene dos componentes principales:
        *    1. descripcion: una cadena que tiene la información 
        *       sobre el evento.
        *    2. prioridad: un número natural que representa la 
        *       prioridad de un evento.
        */
        std::string descripcion;
        int prioridad;

    public:
        friend Evento crearEvento(const std::string& descrip, int prio);
        friend std::string descripcion(const Evento& e);
        friend Evento cambiarDescripcion(const Evento& e, const std::string& nueva);
        friend int suPrioridad(const Evento& e);
        friend Evento cambiarPrioridad(const Evento& e, int pri);
};

#endif 