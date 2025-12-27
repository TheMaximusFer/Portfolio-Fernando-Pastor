/******************************************************************************\
 * Programación 2. Trabajo de asignatura del curso 2023-24
 * Autores: Fernando Pastor Peralta (897113) y Fabrizio Bryan Duarte Jauregui (736857)
 * Ultima revisión: 09/05/2024
 * ----------------------Resumen del problema a resolver:----------------------
 *          
 *  El fichero leido por el programa tendrá los datos correspondientes a cada 
 *  lado de una matriz NxN, cada componente de esta matriz interior representa 
 *  la altura de un edifio y los datos a los lados de la matriz corresponden a 
 *  cuantos edificios se ven desde esa posición si se mira recto a la matriz 
 *  central.
 * 
 * -----------------------------Resumen programa:-----------------------------     
 *  Nuestro programa intentará dar la matriz de edificios de modo de cada vista
 *  se verifique con el resultado, en caso de que no exista solución se comunicará
 *  al usuario.
\******************************************************************************/

#include <iostream>
#include <fstream>
#include "TratarMatriz.hpp"
#include "CrearMatrizEdificios.hpp"
using namespace std;


//ASUMIMOS QUE EL ARCHIVO DE TEXTO QUE METE EL USUARIO TIENE EL FORMATO CORRECTO.
/* Programa el cual leera un fichero de texto facilitado por el usuario en la ejecución del programa, cuyo interior seguirá las directivas 
 * del problema presentado. De este archivo, obtendremos el número de filas/columnas de la matriz cuadrada(NxN) y los datos facilitados en
 * el mismo. Con ellos obtendremos una matriz solución si es posible y la imprimiremos, por lo contrario, si con los datos aportados no 
 * existe solución se lo comunicaremos al usuario.
 * ¡ATENCIÓN!: Este programa no solo verifica las matrices de 1<=N<=4, si no que puede operar con matrices de hasta 1<=N<=9.
 */
int main(int argc, char* argv[]){
    
    ifstream f;
    string nombre_fichero = argv[1];
    bool ok;

    abrirArchivo(f, nombre_fichero, ok);
    if(ok){

        const unsigned N = sacarN(f, nombre_fichero, ok);
        
        unsigned matriz_datos[N][LADOS_VISTAS];                         //Orden de las columnas en sentido horario: arriba[0], derecha[1], abajo[2] e izquierda[3].
        obtenerMatriz(f, N, matriz_datos); 
        f.close();
    
    /* QUITAR COMENTARIO PARA VER LA MATRIZ DE DATOS:
        cout << "El valor de N es: " << N;

        cout << endl << endl;
        for(unsigned i =0; i <= N-1; i++){
            for(unsigned j =0; j <= 3; j++){
                cout << matriz_datos[i][j] << " "; 
            }
            cout << endl;
        }
    */

        unsigned matriz_sol[N*N] = {0};                                 //Usamos un vector ya que no se puede pasar una matriz con columnas dependientes de una variable como parámetro a una función.
                                                                        //v[fila*N+columna]=m[fila][columna] ,columna[0, N-1], fila[0, N-1]
        if(resolverMatriz(matriz_sol, 0, 0, N, matriz_datos, 0)){       //al inicio fila = 0, columna = 0 y casos_verificados = 0
            mostrarMatriz(matriz_sol, N);
            return 0;
        }else{
            cout << "La distribución de sus vistas hace imposible encontrar una solución que construya la matriz de edificios." << endl;
            return 1;
        }

    }else return 1;
}





