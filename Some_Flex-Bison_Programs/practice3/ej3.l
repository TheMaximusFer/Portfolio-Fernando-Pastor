
   // Práctica 3, ej3. AUTORES: Fernando Pastor Peralta (897113) Fabrizio Bryan Duarte Jáuregui (736857)
%{
#include "y.tab.h"
%}
%%
"x" {return(x);}
"y" {return(y);}
\n	{return(EOL);}
[ \t]	{/* ignorar espacios */}
.	{return(yytext[0]); /* caracter inesperado */}
%%
