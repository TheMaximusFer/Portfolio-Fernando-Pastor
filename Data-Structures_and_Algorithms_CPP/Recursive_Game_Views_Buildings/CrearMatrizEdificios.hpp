/******************************************************************************\
 * Programación 2. Trabajo de asignatura del curso 2023-24
 * Autores: Fernando Pastor Peralta (897113) y Fabrizio Bryan Duarte Jauregui (736857)
 * Ultima revisión: 09/05/2024
 * Resumen: Fichero de interfaz de un módulo que ofrece funciones para obtener el 
 *          valor de la matriz solución, obtenida operando con los datos de 
 *          "matriz_datos" siguiendo el modelo del problema. Además también ofrece 
 *          una función de impresión por pantalla de esta matriz solución.
\******************************************************************************/

#pragma once
using namespace std;

const unsigned LADOS_VISTAS = 4;

/*
* Pre: "matriz _datos" tiene en sus filas N datos correspondientes a los datos de arriba, izquierda, abajo y derecha a los que se accede en las columnas con ese orden.
* Además, "fila" representa la posición de la celda que se quiere verficar en el intervalo [0, N-1]. Por último,"matriz_sol" es una matriz unidimensional que sigue la 
* siguiente fórmula: matriz_sol[fila*N+columna].
* Post: Devuelve el valor true si se verifican las vistas correspondientes a la fila de la celda de ["fila"][] de "matriz_sol", o bien, también devuelve true si hay
* un 0 en alguna de las celdas de la fila. Si no, devolveremos false.
*/
bool comprobarVistasFilas(unsigned matriz_sol[], unsigned fila, const unsigned N, unsigned matriz_datos[][4]);


/*
* Pre: "matriz _datos" tiene en sus filas N datos correspondientes a los datos de arriba, izquierda, abajo y derecha a los que se accede en las columnas con ese orden.
* Además, "columna" representa la posición de la celda que se quiere verficar en el intervalo [0, N-1]. Por último,"matriz_sol" es una matriz unidimensional que sigue 
* la siguiente fórmula: matriz_sol[fila*N+columna].
* Post: Devuelve el valor true si se verifican las vistas correspondientes a la columna de la celda de []["columna"] de "matriz_sol", o bien, también devuelve true si hay
* un 0 en alguna de las celdas de la columna. Si no, devolveremos false.
*/
bool comprobarVistasColumnas(unsigned matriz_sol[], unsigned columna, const unsigned N, unsigned matriz_datos[][4]);


/*
* Pre: "matriz _datos" tiene en sus filas N datos correspondientes a los datos de arriba, derecha, abajo y izquierda a los que se accede en las columnas con ese orden.
* Además, "fila" y "columna" representan la posición de la celda que se quiere verficar en el intervalo [0, N-1].
* Post: Devuelve el valor true si se verifican las vistas correspondientes de las filas y columnas de la celda de ["fila"]["columna"] de "matriz_sol". Si no, devolveremos false.
*/
bool comprobarCelda(unsigned matriz_sol[], unsigned fila, unsigned columna, const unsigned N, unsigned matriz_datos[][4]);


/*
* Pre: "matriz _datos" tiene en sus filas N datos correspondientes a los datos de arriba, derecha, abajo y izquierda a los que se accede en las columnas con ese orden.
* Además, "fila" y "columna" representan la posición de la celda que se quiere verficar en el intervalo [0, N-1]. Por último,"matriz_sol" es una matriz unidimensional 
* que sigue la siguiente fórmula: matriz_sol[fila*N+columna].
* Post: Creará en "matriz_sol" la matriz correspondiente a las vistas de "matriz_datos" y devolverá true en el caso de que la matriz a obtener en "matriz_sol" tenga solución, en caso contrario, delvoverá false.
*/
bool resolverMatriz(unsigned matriz_sol[], unsigned fila_inicial, unsigned columna_inicial, const unsigned N, unsigned matriz_datos[][4], unsigned casos_verificados_inicial);


/*
* Pre: "matriz_sol" tiene el resultado correcto del problema y 1<=N<=9 tiene el número correspondiente al número de filas y columnas de la matriz anterior.
* Además,"matriz_sol" es una matriz unidimensional que sigue la siguiente fórmula: matriz_sol[fila*N+columna].
* Post: Enseña por pantalla el contenido de "matriz_sol".
*/
void mostrarMatriz(unsigned matriz_sol[], const unsigned N);

