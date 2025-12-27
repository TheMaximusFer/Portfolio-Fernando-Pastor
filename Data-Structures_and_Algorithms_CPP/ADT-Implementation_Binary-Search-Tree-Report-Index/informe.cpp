// Autor: Hugo López Sola - 875455 y Fernando Pastor Peralta - 897113

#include <iostream>
#include <string>

#include "cola.hpp"
#include "informe.hpp"

// Implementacion de las operaciones del TAD Informe:

Informe crearInforme(const std::string &descrip)
{
    Informe i;
    i.descripcion = descrip;
    i.comentarios = crear<std::string>();

    return i;
}

std::string descripcion(const Informe &i)
{
    return i.descripcion;
}

Informe cambiarDescripcion(Informe &i, const std::string &nueva)
{
    i.descripcion = nueva;
    return i;
}

int comentariosRecibidos(const Informe &i)
{
    return longitud(i.comentarios);
}

Informe registrarComentario(Informe &i, const std::string &coment)
{
    encolar(i.comentarios, coment);
    return i;
}

std::string listarComentarios(Informe &i)
{
    std::string res = "";
    iniciarIterador<std::string>(i.comentarios);
    int p = 1;
    while (existeSiguiente(i.comentarios))
    {
        std::string aux;
        siguiente(i.comentarios, aux);
        res += "[ " + std::to_string(p) + " --> " + aux + " ---];;";
        avanza(i.comentarios);
        p++;
    }
    return res;
}
