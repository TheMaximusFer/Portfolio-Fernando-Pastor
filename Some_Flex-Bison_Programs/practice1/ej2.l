
   // Práctica 1, ej2. AUTORES: Fernando Pastor Peralta (897113) Fabrizio Bryan Duarte Jáuregui (736857)
%{
unsigned contador_hot = 0;
%}
%%
@hotmail printf("@hotmail"); contador_hot++;
%%
int main (){
	int error = yylex ();
	if ( error == 0) { // si escaner finaliza correctamente
		printf ("Total de @hotmail : %d \n", contador_hot);
	}
	return error ;
}
