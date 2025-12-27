
   // Práctica 3, ej3. AUTORES: Fernando Pastor Peralta (897113) Fabrizio Bryan Duarte Jáuregui (736857)
%{
#include <stdio.h>
extern int yylex();
extern int yyerror();
%}

%token x y EOL
%start calclist

%%

calclist: S
	| EOL
	;

S:	EOL calclist
	| C x S
	;

B:	x C y y
	|x C y y EOL calclist
    |x C
	;

C:	x B x 
    |x B x EOL calclist
	|y EOL calclist
    |y
	;
%%

int yyerror(char* s){
	printf("%s\n",s);
	return 0;
}
int main() {
    yyparse();
}
