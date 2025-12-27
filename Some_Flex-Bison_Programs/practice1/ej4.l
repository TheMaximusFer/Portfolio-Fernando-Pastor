
   // Práctica 1, ej4. AUTORES: Fernando Pastor Peralta (897113) Fabrizio Bryan Duarte Jáuregui (736857)
%{
int num_actual;
%}
%%
[0-9] { 
        num_actual = atoi(yytext);
        num_actual++;
        printf("%d", num_actual);
}
[\n] printf("\n\n");
%%
