
   // Práctica 3, ej2.3. AUTORES: Fernando Pastor Peralta (897113) Fabrizio Bryan Duarte Jáuregui (736857)
/* calcMejor.l fichero para la practica 3 de Teoria de la Computacion */
%{
#include "y.tab.h"
int valorRomano(char c);
%}
%%
"+"	{return(ADD);}
"-"	{return(SUB);}
"*"	{return(MUL);}
"/"	{return(DIV);}
"("	{return(OP);}
")"	{return(CP);}
[0-9]+	{yylval = atoi(yytext); return(NUMBER);}
[IVXLCDM]+ { 
    yylval = 0;
    int i = 0;
    while (i < yyleng) {
        int valorActual = valorRomano(yytext[i]);

            // Verificar si forma parte de las cadenas especiales (resta)
            if ((yytext[i] == 'I' && (yytext[i + 1] == 'V' || yytext[i + 1] == 'X')) ||
                (yytext[i] == 'X' && (yytext[i + 1] == 'L' || yytext[i + 1] == 'C')) ||
                (yytext[i] == 'C' && (yytext[i + 1] == 'D' || yytext[i + 1] == 'M'))) {
                int valorSiguiente = 0;
                if (i + 1 < yyleng) {
                    valorSiguiente = valorRomano(yytext[i + 1]);
                }
                yylval += (valorSiguiente - valorActual);
                i += 2; // Saltar ambos caracteres
            } else {
                yylval += valorActual;
                i++; // Avanzar al siguiente carácter
            }
    }
    return(ROMANO);
    }
[ \t]	{/* ignorar espacios */}
\n	{return(EOL);}
.	{return(yytext[0]); /* caracter inesperado */}
%%
int valorRomano(char c) {
    switch (c) {
        case 'I': return 1;
        case 'V': return 5;
        case 'X': return 10;
        case 'L': return 50;
        case 'C': return 100;
        case 'D': return 500;
        case 'M': return 1000;
        default: return 0;
    }
}
