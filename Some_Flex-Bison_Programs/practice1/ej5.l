
   // Práctica 1, ej5. AUTORES: Fernando Pastor Peralta (897113) Fabrizio Bryan Duarte Jáuregui (736857)
%{
#include <stdio.h>
unsigned C = 0;
unsigned L = 0;
unsigned LV = 0;
unsigned V = 0;
unsigned M = 0;
unsigned N = 0;

%}
%%
. C++;
[\n] L++;
^[ \t]*\n LV++; L++; C += yyleng -1;
[a-zA-Z \t]* V++; if(yyleng > M) M = yyleng; C += yyleng;
[0-9  \t]* V++; N++; if(yyleng > M) M = yyleng; C+= yyleng;
%%
int main () {
	int error = yylex ();
	if ( error == 0) { // si escaner finaliza correctamente
		printf ("C: %d \n" , C);
		printf ("L: %d \n" , L);
		printf ("LV: %d \n" , LV);
		printf ("V: %d \n" , V);
		printf ("M: %d \n" , M);
		printf ("N: %d \n" , N);
	}
	return error ;
}
