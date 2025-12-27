// Autor: Hugo López Sola - 875455 y Fernando Pastor Peralta - 897113

/*
Programa de prueba que gestione una colección de informes referentes a números de teléfono denunciados como posibles fuentes de spam telefónico.
El programa gestiona una colección de elementos, donde cada elemento incluye un número de teléfono único (tipo long) y un informe asociado.
La colección garantiza que no haya números de teléfono duplicados. El programa inicia creando una colección vacía y luego lee
instrucciones desde un archivo de texto llamado "entrada.txt". Este archivo contiene una lista de instrucciones y datos asociados,
con la siguiente estructura:

Cada línea de instrucción puede ser:

A: Añadir un nuevo teléfono e informe. Seguido de dos líneas con el número de teléfono y la descripción del informe.
C: Cambiar el informe de un teléfono dado. Seguido de tres líneas: el número de teléfono, la nueva descripción y un comentario adicional.
O: Obtener el informe asociado a un teléfono. Seguido de una línea con el número de teléfono.
E: Comprobar si existe un informe para un teléfono. Seguido de una línea con el número de teléfono.
B: Borrar el teléfono y su informe. Seguido de una línea con el número de teléfono.
LI: Listar los detalles del informe de un teléfono específico. Seguido de una línea con el número de teléfono.
LT: Listar todos los teléfonos e informes. No requiere datos adicionales.

Como resultado de su ejecución, el programa creará un fichero de texto “salida.txt” en el que irá escribiendo el resultado
de cada orden ejecutada, las especificaciones de cada orden se podrá consultar en el main():
*/

#include <iostream>
#include <string>
#include <limits>
#include "informe.hpp"
#include "coleccionMon.hpp"
#include <ostream>
#include <fstream>

int main()
{
    // Definición de los nombres de los archivos de entrada y salida.
    std::string ARCHIVO_ENTRADA = "entrada.txt";
    std::string ARCHIVO_SALIDA = "salida.txt";

    // Crear colección vacía
    ColeccionMon<long, Informe> coleccion = crear<long, Informe>();

    // Leer instrucciones de entrada
    std::ifstream f;
    std::ofstream o;
    f.open(ARCHIVO_ENTRADA);
    o.open(ARCHIVO_SALIDA);
    std::string instruccion;

    while (f >> instruccion)
    {
        /*
         * • Por cada instrucción de tipo “A”, si es posible introducir los datos de un nuevo teléfono y su informe en la
         * colección, se escribirá una línea en el fichero de salida que empiece con la cadena “INTRODUCIDO: ”; si no es
         * posible introducir el teléfono y su informe en la colección, se escribirá una línea en el fichero de salida que
         * empiece con la cadena “NO INTRODUCIDO: ”. En cualquiera de los dos casos, se seguirá escribiendo a
         * continuación, y en la misma línea del fichero, la concatenación de los datos del teléfono y su informe en formato:
         * [ telef ] --- descrip
         * siendo telef y descrip respectivamente, el teléfono y la descripción dados para la instrucción
         */
        if (instruccion == "A")
        {
            long telefono;
            f >> telefono;
            std::string descripcion;
            f.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::getline(f, descripcion, '\n');
            descripcion.erase(descripcion.find_last_not_of(" \n\r\t") + 1); // Eliminar espacios y saltos de línea al final
            if (!(esta(telefono, coleccion)))
            {

                Informe informe = crearInforme(descripcion);
                coleccion = anadir(coleccion, telefono, informe);

                o << "INTRODUCIDO: ";
            }
            else
            {
                o << "NO INTRODUCIDO: ";
            }
            o << "[ " << telefono << " ] --- " << descripcion << std::endl;
        }
        /*
         * • Por cada instrucción de tipo “C”, si es posible actualizar los datos del informe para un teléfono igual al dado
         * en la colección, se escribirá una línea en el fichero de salida que empiece con la cadena “CAMBIADO: ”, seguida
         * de la concatenación de los datos del teléfono y su informe en formato:
         * [ telef ;;; NumAct ;;; NumConsult ] --- descrip --- coment ---
         * siendo telef, descrip y coment respectivamente, el teléfono, la descripción y el comentario dados para la
         * instrucción, y siendo NumAct y NumConsult el número de veces actualizado y el número de veces consultado
         * con los que queda el teléfono en la colección tras resolver la instrucción “C”.
         * Si no es posible actualizar el informe de dicho teléfono en la colección, se escribirá una línea en el fichero de
         * salida que empiece con la cadena “NO CAMBIADO: ”, seguida del número de teléfono dado para la instrucción.
         */
        else if (instruccion == "C")
	{
	    long telefono;
	    f >> telefono;
	    f.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignorar el salto de línea residual

	    Informe informe;

	    if (obtenerVal(telefono, informe, coleccion))
	    {
		std::string descripcion;
		std::getline(f, descripcion); // Leer la descripción normalmente
		descripcion.erase(descripcion.find_last_not_of(" \n\r\t") + 1); // Eliminar espacios y saltos de línea al final
		std::string comentario;
		std::getline(f, comentario);  // Leer el comentario normalmente
		comentario.erase(comentario.find_last_not_of(" \n\r\t") + 1); // Eliminar espacios y saltos de línea al final
		informe = cambiarDescripcion(informe, descripcion);
		informe = registrarComentario(informe, comentario);

		if (!actualizar(coleccion, telefono, informe))
		{
		    o << "NO CAMBIADO: " << telefono << std::endl;
		}
		else
		{
		    int numConsultas, numActualizaciones;
		    obtenerNumActualizaciones(telefono, numActualizaciones, coleccion);
		    obtenerNumConsultas(telefono, numConsultas, coleccion);

		    o << "CAMBIADO: [ " << telefono << " ;;; " << numActualizaciones 
		      << " ;;; " << numConsultas << " ] --- " << descripcion 
		      << " --- " << comentario << " ---" << std::endl;
		}
	    }
	    else
	    {
		o << "NO CAMBIADO: " << telefono << std::endl;
	    }
	}

        /*
         * • Por cada instrucción de tipo “O”, si existía algún informe en la colección para un teléfono igual al dado, se escribirá
         * una línea que empiece con la cadena “LOCALIZADO: ”, seguida de la concatenación de los datos del teléfono
         * encontrado y su informe, en formato:
         * [ telef ;;; NumAct ;;; NumConsult ] --- ( NumComent ) --- descrip ---
         * siendo telef, NumComent y descrip respectivamente, el teléfono, el número total de comentarios y la
         * descripción registrados en el informe encontrado para dicho teléfono en la colección, y siendo NumAct y
         * NumConsult el número de veces actualizado y el número de veces consultado con el que queda el teléfono en la
         * colección tras resolver la instrucción “O”.
         * Si no existía ningún informe en la colección para un teléfono igual al dado, se escribirá en el fichero una línea que
         * empiece con la cadena “NO LOCALIZADO: ”, seguida del número de teléfono dado para la instrucción
         */
        else if (instruccion == "O")
        {
            long telefono;
            f >> telefono;
            Informe informe;
            if (obtenerVal(telefono, informe, coleccion))
            {
                registrarConsulta(telefono, coleccion);
                int numConsultas, numActualizaciones;
                obtenerNumActualizaciones(telefono, numActualizaciones, coleccion);
                obtenerNumConsultas(telefono, numConsultas, coleccion);

                o << "LOCALIZADO: " << "[ " << telefono << " ;;; " << numActualizaciones << " ;;; " << numConsultas << " ] --- ( " << comentariosRecibidos(informe) << " ) --- " << descripcion(informe) << " ---" << std::endl;
            }
            else
            {
                o << "NO LOCALIZADO: " << telefono << std::endl;
            }
        }
        /*
         * • Por cada instrucción de tipo “E”, si existía algún informe en la colección para un teléfono igual al dado, se escribirá
         * una línea que empiece con la cadena “CONOCIDO: ”. Si no existía ningún informe en la colección para un teléfono
         * igual al dado, se escribirá en el fichero una línea que empiece con la cadena “DESCONOCIDO: ”. En cualquiera
         * de los dos casos, se seguirá escribiendo a continuación, y en la misma línea del fichero, el número de teléfono dado
         * para la instrucción.
         */
        else if (instruccion == "E")
        {
            long telefono;
            f >> telefono;
            if (esta(telefono, coleccion))
            {
                o << "CONOCIDO: ";
            }
            else
            {
                o << "DESCONOCIDO: ";
            }
            o << telefono << std::endl;
        }
        /*
         * • Por cada instrucción de tipo “B”, si existía algún informe en la colección para un teléfono igual al dado, se escribirá
         * una línea que empiece con la cadena “BORRADO: ”, y si no existía ningún informe en la colección para un teléfono
         * igual al dado, se escribirá en el fichero una línea que empiece con la cadena “NO BORRADO: ”. En cualquiera de
         * los dos casos, se seguirá escribiendo a continuación, y en la misma línea del fichero, el número de teléfono dado
         * para la instrucción.
         */
        else if (instruccion == "B")
        {
            long telefono;
            f >> telefono;
            if (borrar(telefono, coleccion))
            {
                o << "BORRADO: ";
            }
            else
            {
                o << "NO BORRADO: ";
            }
            o << telefono << std::endl;
        }
        /*
         * • Por cada instrucción de tipo “LI”, se escribirá una línea que empiece con la cadena “****INFORME: ” seguida
         * del número de teléfono utilizado en la operación. Si no existía ningún informe en la colección para un teléfono igual
         * al dado, se escribirá en el fichero una línea que empiece con la cadena “****DESCONOCIDO” finalizando con
         * ello el tratamiento de la instrucción “LI”. Si existe algún informe en la colección para un teléfono igual al dado,
         * a continuación, se escribirá en el fichero de salida: una línea con la descripción registrada en el informe para dicho
         * teléfono en la colección; seguida de una línea con la información detallada de todos los comentarios registrados en
         * el informe para dicho teléfono en la colección, y para cada uno de ellos en formato:
         * [ p ––> coment –––];;
         * siendo coment la cadena registrada como comentario y p el número que representa el puesto de dicho comentario
         * entre todos los comentarios registrados en el informe (puesto 1 para el primer comentario). Los comentarios deberán
         * aparecer listados por orden de puesto creciente.
         * El listado se finalizará escribiendo en el fichero de salida una línea con una cadena con el formato:
         * ****TOTALES: Act->( NumAct ) Cons->( NumConsult ) Coment->( numComent )
         * siendo NumAct y NumConsult el número de veces actualizado y el número de veces consultado con el que queda
         * el teléfono en la colección tras resolver la instrucción “LI”, y siendo numComent el número total de comentarios
         * registrados en el informe encontrado para dicho teléfono en la colección.
         */
        else if (instruccion == "LI")
        {
            long telefono;
            Informe informe;
            f >> telefono;
            o << "****INFORME: " << telefono << std::endl;
            if (obtenerVal(telefono, informe, coleccion))
            {
                registrarConsulta(telefono, coleccion);
                o << descripcion(informe) << std::endl;
                std::string comentarios = listarComentarios(informe);
                if (comentarios != "")
                {
                    o << comentarios << std::endl;
                }
                int numConsultas, numActualizaciones;
                obtenerNumActualizaciones(telefono, numActualizaciones, coleccion);
                obtenerNumConsultas(telefono, numConsultas, coleccion);
                o << "****TOTALES: Act->( " << numActualizaciones << " ) Cons->( " << numConsultas << " ) Coment->( " << comentariosRecibidos(informe) << " )" << std::endl;
            }
            else
            {
                o << "****DESCONOCIDO" << std::endl;
            }
        }
        /*
         * • Por cada instrucción de tipo “LT”, se escribirá una primera línea con la cadena “-----LISTADO: ”, seguida
         * del número total de teléfonos en la colección, y a continuación, por cada teléfono almacenado en la colección y
         * siguiendo un orden creciente según el número de teléfono, se escribirá otra línea con la información de los datos del
         * teléfono y su informe, en formato:
         * [ telef ;;; NumAct ;;; NumConsult ] --- ( NumComent ) --- descrip ---
         * siendo telef, NumComent y descrip respectivamente, el teléfono, el número total de comentarios y la
         * descripción registrados en el informe encontrado para dicho teléfono en la colección, y siendo NumAct y
         * NumConsult el número de veces actualizado y el número de veces consultado con el que se encuentra el teléfono
         * en la colección. El listado finalizará con una línea con la cadena “-----”.
         */
        else if (instruccion == "LT")
        {
            o << "-----LISTADO: " << tamano(coleccion) << std::endl;
            iniciarIterador(coleccion);
            while (existeSiguiente(coleccion))
            {
                long telefono;
                Informe informe;
                siguienteIdent(coleccion, telefono);
                siguienteVal(coleccion, informe);
                int numConsultas, numActualizaciones;
                obtenerNumActualizaciones(telefono, numActualizaciones, coleccion);
                obtenerNumConsultas(telefono, numConsultas, coleccion);
                o << "[ " << telefono << " ;;; " << numActualizaciones << " ;;; " << numConsultas << " ] --- ( " << comentariosRecibidos(informe) << " ) --- " << descripcion(informe) << " ---" << std::endl;
                avanza(coleccion);
            }
            o << "-----" << std::endl;
        }
    }
}
