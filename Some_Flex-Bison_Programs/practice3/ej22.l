
   // Práctica 3, ej2.2. AUTORES: Fernando Pastor Peralta (897113) Fabrizio Bryan Duarte Jáuregui (736857)
/* calcMejor.l fichero para la practica 3 de Teoria de la Computacion */
%{
#include "y.tab.h"
extern int b;
%}
%%
"+"	{return(ADD);}
"-"	{return(SUB);}
"*"	{return(MUL);}
"/"	{return(DIV);}
"("	{return(OP);}
")"	{return(CP);}
"b" {return(B);}
"=" {return(EQU);}
";b" {return(BASEB);}
";" {return(BASE10);}
[0-9]+	{yylval = atoi(yytext); return(NUMBER);}
\n	{return(EOL);}
[ \t]	{/* ignorar espacios */}
.	{return(yytext[0]); /* caracter inesperado */}
%%
