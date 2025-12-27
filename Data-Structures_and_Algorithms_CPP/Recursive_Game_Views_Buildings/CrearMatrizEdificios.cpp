/******************************************************************************\
 * Programación 2. Trabajo de asignatura del curso 2023-24
 * Autores: Fernando Pastor Peralta (897113) y Fabrizio Bryan Duarte Jauregui (736857)
 * Ultima revisión: 09/05/2024
 * Resumen: Módulo que ofrece funciones para obtener el valor de la matriz solución,
 *          obtenida operando con los datos de "matriz_datos" siguiendo el modelo del
 *          problema. Además también ofrece una función de impresión por pantalla de
 *          esta matriz solución.
\******************************************************************************/
#include <iostream>
#include "CrearMatrizEdificios.hpp"
using namespace std;


/*
* Pre: "matriz _datos" tiene en sus filas N datos correspondientes a los datos de arriba, izquierda, abajo y derecha a los que se accede en las columnas con ese orden.
* Además, "fila" representa la posición de la celda que se quiere verficar en el intervalo [0, N-1]. Por último,"matriz_sol" es una matriz unidimensional que sigue la 
* siguiente fórmula: matriz_sol[fila*N+columna].
* Post: Devuelve el valor true si se verifican las vistas correspondientes a la fila de la celda de ["fila"][] de "matriz_sol", o bien, también devuelve true si hay
* un 0 en alguna de las celdas de la fila. Si no, devolveremos false.
*/
bool comprobarVistasFilas(unsigned matriz_sol[], unsigned fila, const unsigned N, unsigned matriz_datos[][LADOS_VISTAS]){
    
    unsigned max_actual = 0, contador_vistos=0;

    //Recorremos fila de izquierda a derecha
    for(unsigned recorrido_columna = 0; recorrido_columna <= N-1; recorrido_columna++){
        if(matriz_sol[fila*N + recorrido_columna] > max_actual){
            max_actual = matriz_sol[fila*N + recorrido_columna];
            contador_vistos++;
        }else if(matriz_sol[fila*N + recorrido_columna] == 0) return true; //Siempre que encontremos un 0 tomaremos la función como verdadera.
    }
    if(contador_vistos == matriz_datos[fila][3]){ //Verificamos la vista de la izquierda.
    //Recorremos fila de derecha a izquierda
        max_actual = 0;
        contador_vistos = 0;

        for(int recorrido_columna = N-1; recorrido_columna >= 0; recorrido_columna--){
            if(matriz_sol[fila*N + recorrido_columna] > max_actual){
                max_actual = matriz_sol[fila*N + recorrido_columna];
                contador_vistos++;
            }
        }

        if(contador_vistos == matriz_datos[fila][1]){ //Verificamos la vista de la derecha.
            return true;
        }else return false;//Si la vista de la derecha no se verifica, devolvemos false.
    }else return false;    //Si la vista de la izquierda no se verifica, devolvemos false.
}


/*
* Pre: "matriz _datos" tiene en sus filas N datos correspondientes a los datos de arriba, izquierda, abajo y derecha a los que se accede en las columnas con ese orden.
* Además, "columna" representa la posición de la celda que se quiere verficar en el intervalo [0, N-1]. Por último,"matriz_sol" es una matriz unidimensional que sigue 
* la siguiente fórmula: matriz_sol[fila*N+columna].
* Post: Devuelve el valor true si se verifican las vistas correspondientes a la columna de la celda de []["columna"] de "matriz_sol", o bien, también devuelve true si hay
* un 0 en alguna de las celdas de la columna. Si no, devolveremos false.
*/
bool comprobarVistasColumnas(unsigned matriz_sol[], unsigned columna, const unsigned N, unsigned matriz_datos[][LADOS_VISTAS]){
    
    unsigned max_actual = 0, contador_vistos=0;
    
    //Recorremos columna de arriba a abajo
    for(unsigned recorrido_fila = 0; recorrido_fila <= N-1; recorrido_fila++){
        if(matriz_sol[recorrido_fila*N + columna] > max_actual){
            max_actual = matriz_sol[recorrido_fila*N + columna];
            contador_vistos++;
        }else if(matriz_sol[recorrido_fila*N + columna] == 0) return true; //Siempre que encontremos un 0 tomaremos la función como verdadera.
    }
    if(contador_vistos == matriz_datos[columna][0]){ //Verificamos la vista de arriba.
    
    //Recorremos fila de abajo a arriba
        max_actual = 0;
        contador_vistos = 0;
        
        for(int recorrido_fila = N-1; recorrido_fila >= 0; recorrido_fila--){
            if(matriz_sol[recorrido_fila*N + columna] > max_actual){
                max_actual = matriz_sol[recorrido_fila*N + columna];
                contador_vistos++;
            }
        }
        if(contador_vistos == matriz_datos[columna][2]){ //Verificamos la vista de abajo.
            return true;
        }else return false;//Si la vista de abajo no se verifica, devolvemos false.
    }else return false;    //Si la vista de arriba no se verifica, devolvemos false.
}


/*
* Pre: "matriz _datos" tiene en sus filas N datos correspondientes a los datos de arriba, derecha, abajo y izquierda a los que se accede en las columnas con ese orden.
* Además, "fila" y "columna" representan la posición de la celda que se quiere verficar en el intervalo [0, N-1].
* Post: Devuelve el valor true si se verifican las vistas correspondientes de las filas y columnas de la celda de ["fila"]["columna"] de "matriz_sol". Si no, devolveremos false.
*/
bool comprobarCelda(unsigned matriz_sol[], unsigned fila, unsigned columna, const unsigned N, unsigned matriz_datos[][LADOS_VISTAS]){
    
    /* QUITAR COMENTARIO PARA VER LA EVOLUCIÓN DE LA MATRIZ:
    for(unsigned filas = 0; filas <= N-1; filas++){
        for (unsigned columnas = 0; columnas <= N-1; columnas++){
            cout << matriz_sol[filas*N + columnas] << " ";
        }
        cout << endl;
    }
    cout << "fila = " << fila << ", columna = " << columna << endl << endl;
    */
    
    return(comprobarVistasFilas(matriz_sol, fila, N, matriz_datos) && comprobarVistasColumnas(matriz_sol, columna, N, matriz_datos));
}

/*
* Pre: "matriz _datos" tiene en sus filas N datos correspondientes a los datos de arriba, derecha, abajo y izquierda a los que se accede en las columnas con ese orden.
* Además, "fila" y "columna" representan la posición de la celda que se quiere verficar en el intervalo [0, N-1]. Por último,"matriz_sol" es una matriz unidimensional 
* que sigue la siguiente fórmula: matriz_sol[fila*N+columna].
* Post: Creará en "matriz_sol" la matriz correspondiente a las vistas de "matriz_datos" y devolverá true en el caso de que la matriz a obtener en "matriz_sol" tenga solución, en caso contrario, delvoverá false.
*/
bool resolverMatriz(unsigned matriz_sol[], unsigned fila, unsigned columna, const unsigned N, unsigned matriz_datos[][LADOS_VISTAS], unsigned casos_verificados){//al inicio fila = 0, columna = 0 y casos_verificados = 0
     
    //Cada vez que se llame a la función, ambas variables se iniciarán con estos valores:
    bool valor = false; 
    unsigned altura_actual = 1;

    if(casos_verificados == N*N){ //Caso base
        valor = true;
    }else{ //Casos recursivos 
        while(N >= altura_actual && !valor){ //Bucle en el que se irán sumando valores a la celda hasta que se verifique o no se cumplan las condicones

            matriz_sol[fila*N + columna] = altura_actual;

            if(comprobarCelda(matriz_sol, fila, columna, N, matriz_datos)){ //En caso de que la celda se verifique, pasamos a la siguiente celda.
                if(columna < N-1){
                    valor = resolverMatriz(matriz_sol, fila, columna + 1, N, matriz_datos, casos_verificados + 1);
                }else valor = resolverMatriz(matriz_sol, fila + 1, 0, N, matriz_datos, casos_verificados + 1);
            }
            altura_actual++;
        }
        if(!valor) matriz_sol[fila*N + columna] = 0; //Si no se cumplen las condiciones, se pone la celda a 0.
    
    }
    return valor; //Este return nos servirá tanto para retornar a la celda anterior como para decir si hay o no solución.
}


/*
* Pre: "matriz_sol" tiene el resultado correcto del problema y 1<=N<=9 tiene el número correspondiente al número de filas y columnas de la matriz anterior.
* Además,"matriz_sol" es una matriz unidimensional que sigue la siguiente fórmula: matriz_sol[fila*N+columna].
* Post: Enseña por pantalla el contenido de "matriz_sol".
*/
void mostrarMatriz(unsigned matriz_sol[], const unsigned N){
    for(unsigned filas = 0; filas <= N-1; filas++){
        for (unsigned columnas = 0; columnas <= N-1; columnas++){
            cout << matriz_sol[filas*N + columnas] << " ";
        }
        cout << endl;
    }
}