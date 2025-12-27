
   // Práctica 1, ej7. AUTORES: Fernando Pastor Peralta (897113) Fabrizio Bryan Duarte Jáuregui (736857)
%%
[I][A]*(([D][A]*){5})*[F] printf("++%s++", yytext);
[I][A]*(([D][A]*){2}){1,4}(([D][A]*){4}((([D][A]*){2}){1,3})*)*[F] printf("+%s+", yytext);
[I][A]*[D][A]*([D][A]*){2}?(([D][A]*){4}((([D][A]*){2}){1,3})*)*[F] printf("-%s-", yytext);
%%
 int main () {
	//MÚLTIPLOS DE 5
	//PAR NO MÚLTIPLOS DE 5
	//IMPAR NO MÚLTIPLOS DE 5
	int error = yylex ();
	return error ;
 }
