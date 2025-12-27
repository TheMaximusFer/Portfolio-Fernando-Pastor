
   // Práctica 3, ej2.1. AUTORES: Fernando Pastor Peralta (897113) Fabrizio Bryan Duarte Jáuregui (736857)
/* calcMejor.l fichero para la practica 3 de Teoria de la Computacion */
%{
#include "y.tab.h"
double pow_(double base, double exponente);
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
"=" {return(EQ);}
[0-9]+	{yylval = atoi(yytext); return(NUMBER);}
[0-9]+"b" {
    yylval = 0;
    if(b >= 2 && b <= 10){
        for(int i = 0; i < yyleng - 1; i++){
            int digito = yytext[i] - '0'; // Convertir el carácter a entero
            if (digito < 0 || digito >= b) {
                printf("Dígito no válido para la base %d.\n", b);
            }
            yylval += digito * pow_(b, yyleng - i - 2);
        }
        return(NUMBER);
    }else{
        printf("La base %d, no está entre 2 y 10.\n", b);
    }
}

\n	{return(EOL);}
[ \t]	{/* ignorar espacios */}
.	{return(yytext[0]); /* caracter inesperado */}
%%
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
