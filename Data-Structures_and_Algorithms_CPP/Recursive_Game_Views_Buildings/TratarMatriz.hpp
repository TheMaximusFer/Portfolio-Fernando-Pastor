/******************************************************************************\
 * Programación 2. Trabajo de asignatura del curso 2023-24
 * Autores: Fernando Pastor Peralta (897113) y Fabrizio Bryan Duarte Jauregui (736857)
 * Ultima revisión: 09/05/2024
 * Resumen: Fichero de interfaz de un módulo que ofrece funciones para obtener el 
 *          valor de N y los datos correspondientes a las vistas que se obtienen 
 *          del archivo que selecciona el usuario.
\******************************************************************************/

#pragma once
using namespace std;

/*
* Pre: --- (true)
* Post: Se intenta abrir el fichero de "nombre_fichero" y se comunica si la operación a tenido éxito o no con "ok". 
*/
void abrirArchivo(ifstream& f,string nombre_fichero, bool& ok);


/*
* Pre: Los datos alrededor de la matriz NxN deben ser 1<=dato<=N, "f" está asocidada correctamente a un fichero que puede leer y "nombre_archivo" 
* tiene la dirección del fichero que lee "f".
* Post: Devuelve el valor de "N", es decir, el número de elementos que tiene la matriz de edificios tanto en filas como en columnas (matriz cuadrada).
*/
unsigned sacarN(ifstream& f, string nombre_fichero, bool ok);


/*
* Pre: "f" está asocidada correctamente a un fichero que puede leer, 1<=N<=9 tiene el número correspondiente al número de filas y columnas de la matriz inteior de "f".
* Post: Guarda en cada fila de "m[][columna]" el valor de cada número distinto de cero de la primera o última fila de la matriz de "f". 
*/
void sacarDatosHorizontales(ifstream& f, const unsigned N, const unsigned columna, unsigned m[][4]);


/*
* Pre: "f" está asocidada correctamente a un fichero que puede leer, 1<=N<=9 tiene el número correspondiente al número de filas y columnas de la matriz inteior de "f".
* Post: Guarda en cada fila de "m" los valores de cada número distinto de cero de una de las filas centrales de la matriz de "f". 
*/
void sacarDatosVerticales(ifstream& f, const unsigned N, unsigned m[][4], unsigned i);


/*
* Pre: "f" está asocidada correctamente a un fichero que puede leer, 1<=N<=9 tiene el número correspondiente al número de filas y columnas de la matriz inteior de "f".
* Post: Asocia a cada fila de m los N valores de vistas asociados a arriba, derecha, abajo e izquierda a los que se puede acceder mediante las columnas en el orden anterior.
*
*/
void obtenerMatriz(ifstream& f, const unsigned N, unsigned m[][4]);