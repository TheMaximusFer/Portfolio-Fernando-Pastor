
 // Práctica 4, th.l AUTORES: Fernando Pastor Peralta (897113) Fabrizio Bryan Duarte Jáuregui (736857)
%{
#include<string.h>
#include "y.tab.h"
%}

%%
[ \t]   /* quitamos espacios y tabuladores */
\n      return(EOL);
[0-9]+  { 
          yylval.nombre = strdup(yytext);
          return(NUMBER);
        }
"->"    return(FLECHA);
"("     return(OP);
")"     return(CP);
","     return(COMA);
";"     return(PUNTOCOMA);       
.       return(yytext[0]); /* caracter inesperado */
%%
