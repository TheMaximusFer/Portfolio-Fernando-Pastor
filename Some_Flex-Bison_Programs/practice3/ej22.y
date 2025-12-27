
   // Práctica 3, ej2.2. AUTORES: Fernando Pastor Peralta (897113) Fabrizio Bryan Duarte Jáuregui (736857)
/* calcMejor.y fichero para la practica 3 de Teoria de la Computacion  */
%{
#include <stdio.h>
extern int yylex();
extern int yyerror();
int b = 10;
%}
%token NUMBER EOL CP OP
%token BASE10 BASEB B EQU
%start calclist
%token ADD SUB
%token MUL DIV
%%

calclist : /* nada */
	| calclist exp BASE10 EOL { printf("=%d\n", $2); }
	| calclist exp BASEB EOL { 
		if(b >= 2 && b <= 10){

			int resultado[32] = {0}; // Almacenará los dígitos del número en la nueva base
			int indice = 0;
			int num = $2;
			if(num != 0){
				// Convertir el número a la base especificada
				while (num > 0) {
					resultado[indice] = num % b;
					num /= b;
					indice++;
				}
				// Imprimir el resultado en orden inverso
				printf("=");
				for (int i = indice - 1; i >= 0; i--) {
					printf("%d", resultado[i]);
				}
			}else printf("=0"); 
			printf("\n");
		}else{
			printf("La base %d, no está entre 2 y 10.\n", b);
		}		
	}
	| calclist B EQU NUMBER EOL { b = $4; }
	;
exp : 	factor 
	| exp ADD factor { $$ = $1 + $3; }
	| exp SUB factor { $$ = $1 - $3; }	
	;
factor : 	factor MUL factorsimple { $$ = $1 * $3; }
		| factor DIV factorsimple { $$ = $1 / $3; }
		| factorsimple
		;
factorsimple : 	OP exp CP { $$ = $2; }
		| NUMBER 
		;
%%
int yyerror(char* s) {
   printf("\n%s\n", s);
   return 0;
}
int main() {
  yyparse();
}

