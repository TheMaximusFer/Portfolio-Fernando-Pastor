
   // Práctica 2, ej1. AUTORES: Fernando Pastor Peralta (897113) Fabrizio Bryan Duarte Jáuregui (736857)
%{
int contadorH = 0;
%}

%x COMENTARIO
%x BUSCARHREF
%x HREFENCONTRADO
%x CODIGOH

%%
.
[\n]
"<!--" BEGIN(COMENTARIO);
[<][Hh][1-6] contadorH++; BEGIN(CODIGOH);
[<][Aa][^a-zA-Z] BEGIN(BUSCARHREF);

<CODIGOH>{
	. 
	[\n]
	"<!--" BEGIN(COMENTARIO);
	[<][/][Hh][1-6][>] BEGIN(INITIAL); 
	}

<COMENTARIO>{
	.
	[\n]
	"-->" BEGIN(INITIAL);
	}
<BUSCARHREF>{
	.
	[\n]
	"<!--" BEGIN(COMENTARIO);
	[Hh][Rr][Ee][Ff][ ]*[=][ ]*\" BEGIN(HREFENCONTRADO);
	[<][/][Aa][>] BEGIN(INITIAL);
	}
<HREFENCONTRADO>{
	.
	[\n]
	[^\"]+ printf("%s\n", yytext); BEGIN(BUSCARHREF);
	}
	
%%
int main () {
	int error = yylex ();
	if ( error == 0) { // si escaner finaliza correctamente
		printf ("H: %d \n", contadorH);
	}
	return error ;
}
