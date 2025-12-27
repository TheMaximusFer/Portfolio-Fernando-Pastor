
 // Práctica 4, th.y AUTORES: Fernando Pastor Peralta (897113) Fabrizio Bryan Duarte Jáuregui (736857)
%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
extern int yylex();
extern int yyerror();

#define PALOS 3
#define DIM 27
#define BUFF 4000

// Tabla de adyacencia.
char* tablaTransiciones[DIM][DIM];
char* fila_tablaTransiciones;

// Inicializa una tabla cuadrada DIM x DIM con la cadena vacía.
void iniTabla(char* tabla[DIM][DIM]) {
	for (int i = 0; i < DIM; i++) {
		for (int j = 0; j < DIM; j++) {
			tabla[i][j] = "";
		}
	}
}

/*
 * Multiplicación simbólica de matrices cuadradas:
 * DIM x DIM: res = t1*t2
 *
 * CUIDADO: res DEBE SER UNA TABLA DISTINTA A t1 y t2
 * Por ejemplo, NO SE DEBE USAR en la forma:
 *           multiplicar(pot, t, pot); //mal
 */
void multiplicar(char* t1[DIM][DIM], char* t2[DIM][DIM], char* res[DIM][DIM]) {
	for (int i = 0; i < DIM; i++) {
		for (int j = 0; j < DIM; j++) {
			res[i][j] = (char*) calloc(BUFF, sizeof(char));
			for (int k = 0; k < DIM; k++) {
				if (strcmp(t1[i][k],"")!=0 && strcmp(t2[k][j],"") != 0) {
					strcat(strcat(res[i][j],t1[i][k]),"-");
					strcat(res[i][j],t2[k][j]);
				}
			}
		}
	}
}

/* 
* Copia la tabla orig en la tabla copia.
*/
void copiar(char* orig[DIM][DIM], char* copia[DIM][DIM]) {
	for (int i = 0; i < DIM; i++) {
		for (int j = 0; j < DIM; j++) {
			copia[i][j] = strdup(orig[i][j]);
		}
	}
}

/*
* Función que devuelve el resultado de base^exponente, con exponente >= 0 y base >= 0.
*/
double pow_(double base, double exponente) {
    if (exponente == 0) {
        return 1; // Cualquier número elevado a 0 es 1
    }
    
    if (base == 0 && exponente > 0) {
        return 0; // 0 elevado a cualquier número positivo es 0
    }

    double result = 1.0;
    for (int i = 0; i < (int)exponente; ++i) {
        result *= base; // Multiplicación repetida
    }

    return result;
}

/*
* Devuelve "numero" en base "baseEntrada" en la base "baseSalida".
*/ 
int convertirBase(int numero, int baseEntrada, int baseSalida) {
    // Convertimos el número de la base de entrada a base decimal.
    int decimal = 0, resultado = 0, factor = 1, i = 0, digito;
    while (numero != 0) {
        decimal += (numero % 10) * pow_(baseEntrada, i);
        numero /= 10;
        i++;
    }

    // Convertimos el número en base decimal a la base de salida.
    while (decimal != 0) {
        digito = decimal % baseSalida;
        resultado += digito * factor;
        decimal /= baseSalida;
        factor *= 10;
    }

    return resultado;
}

/*
* Liberar la memoria dinámica usada.
*/
void liberarTabla(char* tabla[DIM][DIM]) {
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            if (tabla[i][j] != NULL && tabla[i][j][0] != '\0') {
                free(tabla[i][j]);
            }
        }
    }
}


%}

%union {
    char* nombre;
}

%token EOL NUMBER FLECHA OP CP COMA PUNTOCOMA
%start linea

%type<nombre> NUMBER

%%
linea: /*nada*/ |
       linea estado FLECHA conjuntotransiciones PUNTOCOMA EOL;

estado: NUMBER {
    fila_tablaTransiciones = strdup($1);
}

conjuntotransiciones: transicion | 
                      conjuntotransiciones COMA transicion;


transicion: NUMBER OP NUMBER CP {
    tablaTransiciones[convertirBase(atoi(fila_tablaTransiciones), PALOS, 10)]
                     [convertirBase(atoi($1), PALOS, 10)] = strdup($3);
}
%%

int yyerror(char* s) {
   printf("\n%s\n", s);
   return 0;
}

int main(int argc, char* argv[]){
    
    //Si el usuario no conoce del uso del ejecutable, le informamos.
    if (argc != 4) {
        printf("Uso: %s <fichero_entrada> <nodo_inicial> <nodo_final>\n\n", argv[0]);
        printf("Tomando comportamiento por defecto:\n");
        printf("-----------------------------------\n");
        printf("* Fichero: thP3D3.txt\n");
        printf("* Nodo inicial: 000\n");
        printf("* Nodo final: 222\n");
        printf("-----------------------------------\n\n");
        argv[1] = "thP3D3.txt";
        argv[2] = "000";
        argv[3] = "222";
    }else{
        printf("\nParámetros tomados:\n");
        printf("-----------------------------------\n");
        printf("* Fichero: %s\n", argv[1]);
        printf("* Nodo inicial: %s\n", argv[2]);
        printf("* Nodo final: %s\n", argv[3]);
        printf("-----------------------------------\n\n");
    }


    //Intentamos abrir el fichero en modo de lectura.
    FILE *fichero = fopen(argv[1], "r");
    if (fichero == NULL) {
        perror("Error al abrir el fichero");
        return 1; // Salir con un código de error.
    }
    
    //inicializar lista transiciones
    int total = 0;

    //inicializar tabla de adyacencia
    iniTabla(tablaTransiciones);

    //nodo inicial
    int nodoOrig = convertirBase(atoi(argv[2]), PALOS, 10);

    //nodo final
    int nodoFin = convertirBase(atoi(argv[3]), PALOS, 10);
    
    extern FILE *yyin;
    yyin = fichero;
        
        int error = yyparse();

        if (error == 0) {
            if(nodoOrig != nodoFin){
                //Matriz para guardar la potencia
                char* pot[DIM][DIM];
                char* res[DIM][DIM];
                iniTabla(pot);
                iniTabla(res);
                copiar(tablaTransiciones,pot);
                copiar(pot, res);
            
                /*Calcular movimientos de nodoOrig a nodoFin,
                calculando las potencias sucesivas de tablaTransiciones*/
                while(strcmp(pot[nodoOrig][nodoFin], "") == 0){
                    multiplicar(tablaTransiciones, pot, res);
                    copiar(res, pot);
                    total ++;
                }

                printf("Nodo inicial  : %s\n", argv[2]);
                printf("Movimientos   : %s\n", pot[nodoOrig][nodoFin]);
                printf("Nodo final    : %s\n", argv[3]);
                printf("Se han pasado por %d estados (%d transiciones).\n", total+2, total+1);
                
                liberarTabla(pot);
                liberarTabla(res);
            }else{
                printf("No se permite mismo nodo inicial y final.\n");
                return 1;
            }
        }

        liberarTabla(tablaTransiciones);

        fclose(fichero);
        return error;
}
