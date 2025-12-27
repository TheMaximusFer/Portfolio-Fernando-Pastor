
   // Práctica 3, ej2.3. AUTORES: Fernando Pastor Peralta (897113) Fabrizio Bryan Duarte Jáuregui (736857)
/* calcMejor.y fichero para la practica 3 de Teoria de la Computacion  */
%{
#include <stdio.h>
extern int yylex();
extern int yyerror();
%}
%token NUMBER EOL CP OP
%token ROMANO
%start calclist
%token ADD SUB
%token MUL DIV
%%
calclist : /* nada */
	| calclist exp EOL { printf("=%d\n", $2); }
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
		| ROMANO
		;
%%
int yyerror(char* s) {
   printf("\n%s\n", s);
   return 0;
}
int main() {
  yyparse();
}

