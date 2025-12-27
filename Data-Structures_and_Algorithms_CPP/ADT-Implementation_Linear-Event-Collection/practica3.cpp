/* 
* Autores: 
*    - Daniel Jal Burguete, 815450 
*    - Fernado Pastor Peralta, 897113
*/

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include "eventos.hpp"
#include "coleccionesInterdependientes.hpp"

using namespace std;

// Especialización de las funciones de comparación para std::string
template<> bool igual<string>(const string& s1, const string& s2) { return s1 == s2; }
template<> bool anterior<string>(const string& s1, const string& s2) { return s1 < s2; }

void pillarLinea(ifstream& f, string& linea) {
    getline(f, linea);
    linea.erase(linea.find_last_not_of("\n\r") + 1); // Eliminar saltos de línea al final, para eliminar espacios linea.erase(linea.find_last_not_of(" \n\r\t") + 1);
}

/* 
 * Pre:  f_in y f_out deben estar abiertos, coleccion inicializada.
 * Post: Procesa una orden A: añade un evento independiente o dependiente según tipo, y escribe el resultado en f_out.
 */
void procesar_A(ifstream& f_in, ofstream& f_out, ColecInterdep<string, Evento>& coleccion) {
    string nombre, desc, tipo, nomSup;
    int prio;
    
    pillarLinea(f_in, nombre);
    pillarLinea(f_in, desc);
    f_in >> prio;
    f_in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    pillarLinea(f_in, tipo);
    pillarLinea(f_in, nomSup);

    bool existia_antes = existe(coleccion, nombre);
    bool operacion_exitosa = false;

    if (!existia_antes) {
        Evento ev = crearEvento(desc, prio);
        if (tipo == "INDependiente") {
            añadirIndependiente(coleccion, nombre, ev);
            operacion_exitosa = true;
        } else if (tipo == "DEPendiente") {
            if (existe(coleccion, nomSup)) {
                añadirDependiente(coleccion, nombre, ev, nomSup);
                operacion_exitosa = true;
            }
        }
    }
    
    if (operacion_exitosa) {
         f_out << "INTRODUCIDO: ";
    } else {
         f_out << "NO INTRODUCIDO: ";
    }

    if (tipo == "INDependiente") {
        f_out << "[ " << nombre << " ] --- " << desc << " --- ( " << prio << " )" << endl;
    } else {
        f_out << "[ " << nombre << " -de-> " << nomSup << " ] --- " << desc << " --- ( " << prio << " )" << endl;
    }
}

/* 
 * Pre:  f_in y f_out deben estar abiertos, coleccion inicializada.
 * Post: Procesa una orden C: cambia descripción y prioridad de un evento si existe, y escribe el resultado en f_out.
 */
void procesar_C(ifstream& f_in, ofstream& f_out, ColecInterdep<string, Evento>& coleccion) {
    string nombre, desc;
    int prio;
    pillarLinea(f_in, nombre);
    pillarLinea(f_in, desc);
    f_in >> prio;
    f_in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (existe(coleccion, nombre)) {
        Evento ev_nuevo = crearEvento(desc, prio);
        actualizarVal(coleccion, nombre, ev_nuevo);
        
        f_out << "CAMBIADO: ";
        int numDep = obtenerNumDependientes(coleccion, nombre);
        if (existeIndependiente(coleccion, nombre)) {
            f_out << "[ " << nombre << " --- " << numDep << " ] --- " << desc << " --- ( " << prio << " )" << endl;
        } else {
            string nomSup = obtenerSupervisor(coleccion, nombre);
            f_out << "[ " << nombre << " -de-> " << nomSup << " ;;; " << numDep << " ] --- " << desc << " --- ( " << prio << " )" << endl;
        }
    } else {
        f_out << "NO CAMBIADO: " << nombre << endl;
    }
}

/* 
 * Pre:  f_in y f_out deben estar abiertos, coleccion inicializada.
 * Post: Procesa una orden O: obtiene y escribe información de un evento si existe.
 */
void procesar_O(ifstream& f_in, ofstream& f_out, const ColecInterdep<string, Evento>& coleccion) {
    string nombre;
    pillarLinea(f_in, nombre);

    if (existe(coleccion, nombre)) {
        f_out << "LOCALIZADO: ";
        Evento ev = obtenerVal(coleccion, nombre);
        int numDep = obtenerNumDependientes(coleccion, nombre);
        if (existeIndependiente(coleccion, nombre)) {
            f_out << "[ " << nombre << " --- " << numDep << " ] --- " << descripcion(ev) << " --- ( " << suPrioridad(ev) << " )" << endl;
        } else {
            string nomSup = obtenerSupervisor(coleccion, nombre);
            f_out << "[ " << nombre << " -de-> " << nomSup << " ;;; " << numDep << " ] --- " << descripcion(ev) << " --- ( " << suPrioridad(ev) << " )" << endl;
        }
    } else {
        f_out << "NO LOCALIZADO: " << nombre << endl;
    }
}

/* 
 * Pre:  f_in y f_out deben estar abiertos, coleccion inicializada.
 * Post: Procesa una orden E: determina si un evento es independiente, dependiente o desconocido, y escribe el resultado.
 */
void procesar_E(ifstream& f_in, ofstream& f_out, const ColecInterdep<string, Evento>& coleccion) {
    string nombre;
    pillarLinea(f_in, nombre);
    if(existeIndependiente(coleccion, nombre)){
        f_out << "INDEPendiente: " << nombre << endl;
    } else if (existeDependiente(coleccion, nombre)) {
        f_out << "DEPendiente: " << nombre << endl;
    } else {
        f_out << "DESCONOCIDO: " << nombre << endl;
    }
}

/*
 * Pre:  f_in y f_out deben estar abiertos, coleccion inicializada.
 * Post: Procesa una orden I: hace independiente un evento si es dependiente, y escribe el resultado en f_out.
 */
void procesar_I(ifstream& f_in, ofstream& f_out, ColecInterdep<string, Evento>& coleccion) {
    string nombre;
    pillarLinea(f_in, nombre);
    if(existeDependiente(coleccion, nombre)) {
        hacerIndependiente(coleccion, nombre);
        f_out << "INDEPENDIZADO: " << nombre << endl;
    } else if (existeIndependiente(coleccion, nombre)) {
        f_out << "YA ERA INDepend.: " << nombre << endl;
    } else {
        f_out << "NO INDEPENDIZADO: " << nombre << endl;
    }
}

/*
 * Pre: f_in y f_out deben estar abiertos, coleccion inicializada.
 * Post: Procesa una orden D: hace dependiente un evento si es independiente y si el superior existe, escribe el resultado en f_out.
 */
void procesar_D(ifstream& f_in, ofstream& f_out, ColecInterdep<string, Evento>& coleccion) {
    string nombre1, nombre2;
    pillarLinea(f_in, nombre1);
    pillarLinea(f_in, nombre2);

    if(existe(coleccion, nombre1) && existe(coleccion, nombre2)) {
        f_out << "INTENTANDO hacer depend.: ";
        hacerDependiente(coleccion, nombre1, nombre2);
    } else {
        f_out << "IMPOSIBLE hacer depend.: ";
    }
    f_out << nombre1 << " -de-> " << nombre2 << endl;
}

/*
 * Pre:  f_in y f_out deben estar abiertos, coleccion inicializada.
 * Post: Procesa una orden B: borra un evento si es independiente y si no tiene dependientes, escribe el resultado en f_out.
 */
void procesar_B(ifstream& f_in, ofstream& f_out, ColecInterdep<string, Evento>& coleccion) {
    string nombre;
    pillarLinea(f_in, nombre);
    if(existe(coleccion, nombre) && obtenerNumDependientes(coleccion, nombre) == 0){
        borrar(coleccion, nombre);
        f_out << "BORRADO: " << nombre << endl;
    } else {
        f_out << "NO BORRADO: " << nombre << endl;
    }
}

/* 
 * Pre:  f_out debe estar abierto, coleccion inicializada.
 * Post: Procesa una orden LT: lista todos los eventos de la colección en orden, con formato detallado.
 */
void procesar_LT(ofstream& f_out, ColecInterdep<string, Evento>& coleccion) {
    
    f_out << "-----LISTADO: " << tamaño(coleccion) << endl;
    iniciarIterador(coleccion);
    while (existeSiguiente(coleccion)) {
        string nombre = siguienteIdent(coleccion);
        Evento ev = siguienteVal(coleccion);
        int numDep = siguienteNumDependientes(coleccion);

        if (siguienteDependiente(coleccion)) {
            string nomSup = siguienteSuperior(coleccion);
            
            f_out << "[ " << nombre << " -de-> " << nomSup << " ;;; " << numDep << " ] --- " << descripcion(ev) << " --- ( " << suPrioridad(ev) << " )" << endl;
        } else {
            f_out << "[ " << nombre << " --- " << numDep << " ] --- " << descripcion(ev) << " --- ( " << suPrioridad(ev) << " )" << endl;
        }
        avanza(coleccion);
    }
    f_out << "-----" << endl;
}

/*
 * Pre:  f_in y f_out deben estar abiertos, coleccion inicializada.
 * Post: Procesa una orden LD: lista los dependientes directos de nombreSup, con formato detallado.
 */
void procesar_LD(ifstream& f_in, ofstream& f_out, ColecInterdep<string, Evento>& coleccion) {
    string nombreSup;
    pillarLinea(f_in, nombreSup);
    f_out << "****DEPENDIENTES: " << nombreSup << endl;

    if (!existe(coleccion, nombreSup)) {
        f_out << "****DESCONOCIDO" << endl;
        return;
    }

    Evento evSup = obtenerVal(coleccion, nombreSup);
    int numDepSup = obtenerNumDependientes(coleccion, nombreSup);

    if (existeIndependiente(coleccion, nombreSup)) {
        f_out << "[ " << nombreSup << " --- " << numDepSup << " ] --- " << descripcion(evSup) << " --- ( " << suPrioridad(evSup) << " ) ****" << endl;
    } else {
        string superDelSup = obtenerSupervisor(coleccion, nombreSup);
        f_out << "[ " << nombreSup << " -de-> " << superDelSup << " ;;; " << numDepSup << " ] --- " << descripcion(evSup) << " --- ( " << suPrioridad(evSup) << " ) ****" << endl;
    }
    
    int p = 1;
    iniciarIterador(coleccion);
    while (existeSiguiente(coleccion)) {
        if (siguienteDependiente(coleccion) && igual(siguienteSuperior(coleccion), nombreSup)) {
            string nombreDep = siguienteIdent(coleccion);
            Evento evDep = siguienteVal(coleccion);
            int numDepDep = siguienteNumDependientes(coleccion);
            
            if (existeIndependiente(coleccion, nombreDep)) { // No deberia pasar nunca.
                 f_out << "[" << p << " -> " << nombreDep << " ;;; " << numDepDep << " ] --- " << descripcion(evDep) << " --- ( " << suPrioridad(evDep) << " ) ;;;;" << endl;
            } else {
                 string superDelDep = obtenerSupervisor(coleccion, nombreDep);
                 f_out << "[" << p << " -> " << nombreDep << " -de-> " << superDelDep << " ;;; " << numDepDep << " ] --- " << descripcion(evDep) << " --- ( " << suPrioridad(evDep) << " ) ;;;;" << endl;
            }
            p++;
        }
        avanza(coleccion);
    }

    f_out << "****FINAL dependientes -de-> " << nombreSup << endl;
}

/* 
 * Programa principal: toma como primer argumento un fichero de entrada y como segundo un fichero de 
 * salida, procesando cada línea de la entrada, con la forma de instrucción (A, C, O, E, I, D, B, LT, LD) 
 * seguida de sus parámetros necesarios, y escribiendo los resultados en el fichero de salida. 
 * 
 * A continuación, se especifican qué resultados producen en la salida cada una de las instrucciones 
 * en cada línea de la entrda:
 *  - A: Añade un evento independiente o dependiente según tipo. Escribe "INTRODUCIDO: " o "NO INTRODUCIDO: "
 *  - C: Cambia descripción y prioridad de un evento si existe. Escribe "CAMBIADO: " o "NO CAMBIADO: "
 *  - O: Obtiene y escribe información de un evento si existe. Escribe "LOCALIZADO: " o "NO LOCALIZADO: "
 *  - E: Determina si un evento es independiente, dependiente o desconocido. Escribe "INDEPendiente: ", "DEPendiente: " o "DESCONOCIDO: "
 *  - I: Hace independiente un evento si es dependiente. Escribe "INDEPENDIZADO: ", "YA ERA INDepend.: " o "NO INDEPENDIZADO: "
 *  - D: Hace dependiente un evento si es independiente y si el superior existe. Escribe "INTENTANDO hacer depend.: " seguido de "nombre1 -de-> nombre2" o 
 *      "IMPOSIBLE hacer depend.: " seguido de "nombre1 -de-> nombre2"
 *  - B: Borra un evento si es independiente y si no tiene dependientes. Escribe "BORRADO: " o "NO BORRADO: "
 *  - LT: Lista todos los eventos de la colección en orden, con formato detallado.
 *  - LD: Lista los dependientes directos de un evento, con formato detallado. Si el evento no existe, escribe "****DESCONOCIDO".
 *  
 *  Descripción de argumentos de cada instrucción:
 *  - A: nombre (string), descripción (string), prioridad (int), tipo (string), nombreSuperior (string)
 *  - C: nombre (string), nueva descripción (string), nueva prioridad (int)
 *  - O: nombre (string)
 *  - E: nombre (string)
 *  - I: nombre (string)
 *  - D: nombre1 (string), nombre2 (string)
 *  - B: nombre (string)
 *  - LT: (sin argumentos)
 *  - LD: nombreSuperior (string)
 */  
int main() {

    string fichero_entrada = "entrada.txt";
    string fichero_salida = "salida.txt";

    ifstream f_in(fichero_entrada);
    if (!f_in.is_open()) {
        cerr << "Error: No se pudo abrir el fichero entrada.txt" << endl;
        return 1;
    }

    ofstream f_out(fichero_salida);
    if (!f_out.is_open()) {
        cerr << "Error: No se pudo crear el fichero salida.txt" << endl;
        return 1;
    }

    ColecInterdep<string, Evento> coleccion = crear<string, Evento>();
    string instruccion, salto;

    while (f_in >> instruccion) {
        pillarLinea(f_in, salto); 

        if (instruccion == "A")         procesar_A(f_in, f_out, coleccion);
        else if (instruccion == "C")    procesar_C(f_in, f_out, coleccion);
        else if (instruccion == "O")    procesar_O(f_in, f_out, coleccion);
        else if (instruccion == "E")    procesar_E(f_in, f_out, coleccion);
        else if (instruccion == "I")    procesar_I(f_in, f_out, coleccion);
        else if (instruccion == "D")    procesar_D(f_in, f_out, coleccion);
        else if (instruccion == "B")    procesar_B(f_in, f_out, coleccion);
        else if (instruccion == "LT")   procesar_LT(f_out, coleccion);
        else if (instruccion == "LD")   procesar_LD(f_in, f_out, coleccion);
    }

    f_in.close();
    f_out.close();

    borrarTodo(coleccion);

    return 0;
}