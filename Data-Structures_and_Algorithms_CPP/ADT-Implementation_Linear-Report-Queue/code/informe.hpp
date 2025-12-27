// Autor: Hugo López Sola - 875455 y Fernando Pastor Peralta - 897113

#ifndef INFORME_HPP
#define INFORME_HPP
#include <iostream>
#include "cola.hpp"

// Interfaz del TAD Informe. Pre-declaraciones:

/*
* Los valores del TAD informe representarán tuplas formadas como:
* (descripción, comentarios) siendo la descripción una cadena y comentarios una cola que contendrá
* todos los comentarios registrados en dicho informe (siendo cada comentario una cadena, y que
* deberán quedar registrados según el orden de registro o recepción del comentario).
* Declaración adelantada del TAD Informe:
*/
struct Informe;


/* Dada una cadena descrip devuelve un informe compuesto con descripción descrip y sin ningún
 comentario registrado.
 */
Informe crearInforme(const std::string &descrip);

/* Dado un informe i formado como (D,C) devuelve la cadena D, i.e. la descripción en el informe i.
 */
std::string descripcion(const Informe &i);

/* Dado un informe i formado como (D,C) devuelve un informe igual al compuesto como (nueva,C).
 */
Informe cambiarDescripcion(Informe &i, const std::string &nueva);

/* Dado un informe i formado como (D,C), devuelve el número total de comentarios registrados en el
 informe i, es decir, longitud(C).
 */
int comentariosRecibidos(const Informe &i);

/* Dado un informe i formado como (D,C) y un comentario coment, devuelve un informe igual al
 resultante de añadir el comentario coment a los registrados en el informe i, es decir, un informe
 igual al compuesto como (D,encolar(C,coment)).
 */
Informe registrarComentario(Informe &i, const std::string &coment);

/* Dado un informe i formado como (D,C), devuelve una cadena formada con la información detallada
 de todos los comentarios registrados en su C y en el mismo orden en el que fueron registrados,
 empezando por el primero que se registró y acabando por el último registrado.
 Si el informe i no tiene ningún comentario registrado la cadena resultado será la cadena vacía,
 en caso contrario la cadena resultante se construirá con el formato que se describe a continuación.
 La cadena contendrá la concatenación de la información detallada de cada uno de los comentarios
 registrados en C, y para cada uno de ellos se incluirá en la cadena resultante su información con
 el siguiente formato:
[ p ––> coment –––];;
 siendo coment la cadena registrada como comentario y p el número que representa el puesto de dicho
 comentario entre todos los comentarios registrados en el informe (siendo el puesto 1 para el primer
 comentario).
 */
std::string listarComentarios(const Informe &i);

// Declaración:
struct Informe
{
private: // Representación de los valores del TAD
    std::string descripcion;
    Cola<std::string> comentarios;

public:
    friend Informe crearInforme(const std::string &descrip);
    friend std::string descripcion(const Informe &i);
    friend Informe cambiarDescripcion(Informe &i, const std::string &nueva);
    friend int comentariosRecibidos(const Informe &i);
    friend Informe registrarComentario(Informe &i, const std::string &coment);
    friend std::string listarComentarios(Informe &i);
};

#endif // INFORME_HPP