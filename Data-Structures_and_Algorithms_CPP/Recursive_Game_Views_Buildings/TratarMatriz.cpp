/******************************************************************************\
 * Programación 2. Trabajo de asignatura del curso 2023-24
 * Autores: Fernando Pastor Peralta (897113) y Fabrizio Bryan Duarte Jauregui (736857)
 * Ultima revisión: 09/05/2024
 * Resumen: Módulo que ofrece funciones para obtener el valor de N y los datos
 *          correspondientes a las vistas que se obtienen del archivo que selecciona
 *          el usuario.
\******************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
using namespace std;


/*
* Pre: --- (true)
* Post: Se intenta abrir el fichero de "nombre_fichero" y se comunica si la operación a tenido éxito o no con "ok". 
*/
void abrirArchivo(ifstream& f,string nombre_fichero, bool& ok){
    f.open(nombre_fichero);

    if(f.is_open()){
        ok = true;
    }else{ 
        cerr << "No se ha podido acceder a \"" << nombre_fichero << "\"." << endl; 
        ok = false;
    }
}


/*
* Pre: Los datos alrededor de la matriz NxN deben ser 1<=dato<=N, "f" está asocidada correctamente a un fichero que puede leer y "nombre_archivo" 
* tiene la dirección del fichero que lee "f".
* Post: Devuelve el valor de "N", es decir, el número de elementos que tiene la matriz de edificios tanto en filas como en columnas (matriz cuadrada).
*/
unsigned sacarN(ifstream& f, string nombre_fichero, bool ok){

    string provisional;
    unsigned N;

    getline(f, provisional, ' ');           //Sacamos el primer 0
    getline(f, provisional, '0');           //Sacamos todo lo que haya hasta el último 0
    N = provisional.length()/2;             //Provisional tendrá 2*N carácteres

    f.close();
    abrirArchivo(f, nombre_fichero, ok);

    return N;
}


/*
* Pre: "f" está asocidada correctamente a un fichero que puede leer, 1<=N<=9 tiene el número correspondiente al número de filas y columnas de la matriz inteior de "f".
* Post: Guarda en cada fila de "m[][columna]" el valor de cada número distinto de cero de la primera o última fila de la matriz de "f". 
*/
void sacarDatosHorizontales(ifstream& f, const unsigned N, const unsigned columna, unsigned m[][4]){
    
    string provisional;

    getline(f, provisional, ' ');           //Sacamos el primer 0
    for(unsigned i = N; i >= 1; i--){       //Obtenemos los números centrales
        getline(f, provisional, ' ');
        m[N-i][columna] = stoi(provisional);
    }
    getline(f, provisional, '\n');          //Sacamos el 0 restante y pasamos a la siguiente fila
}


/*
* Pre: "f" está asocidada correctamente a un fichero que puede leer, 1<=N<=9 tiene el número correspondiente al número de filas y columnas de la matriz inteior de "f".
* Post: Guarda en cada fila de "m" los valores de cada número distinto de cero de una de las filas centrales de la matriz de "f". 
*/
void sacarDatosVerticales(ifstream& f, const unsigned N, unsigned m[][4], unsigned i){
    
    string provisional;

    getline(f, provisional, ' ');
    m[N-i][3] = stoi(provisional);           //Obtenemos el número de la izquierda

    for(unsigned j = 1; j <= N; j++) getline(f, provisional, ' '); //Sacamos los 0

    getline(f, provisional, '\n');          //Obtenemos el número de la derecha y pasamos a la siguiente fila 
    m[N-i][1] = stoi(provisional); 
}


/*
* Pre: "f" está asocidada correctamente a un fichero que puede leer, 1<=N<=9 tiene el número correspondiente al número de filas y columnas de la matriz inteior de "f".
* Post: Asocia a cada fila de m los N valores de vistas asociados a arriba, derecha, abajo e izquierda a los que se puede acceder mediante las columnas en el orden anterior.
*
*/
void obtenerMatriz(ifstream& f, const unsigned N, unsigned m[][4]){
    /*
    * Sacar primera fila (sacar 0 y N números), luego sacar N veces el primer y último elemento de cada fila y por último
    * volver a la misma función anterior que saca la primera fila para sacar la última.
    */

    sacarDatosHorizontales(f, N, 0, m);
    
    for(unsigned i = N; i >= 1; i--){
        sacarDatosVerticales(f, N, m, i);
    }
    sacarDatosHorizontales(f, N, 2, m);
}