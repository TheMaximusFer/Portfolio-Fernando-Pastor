#------------------------------------------------------------------------------
#  Programación 2. Trabajo de asignatura del curso 2023-24
#  Autores: Fernando Pastor Peralta (897113) y Fabrizio Bryan Duarte Jauregui (736857)
#  Ultima revisión: 09/05/2024
#  Resumen: Makefile del programa coloca.
#------------------------------------------------------------------------------

#---------------------------------------------------------
# Definición de macros
CC = g++                 # compilador
CPPFLAGS = -std=c++11    # opciones de compilación
RM = rm -f               # comando para borrar ficheros

all: coloca

# Compilación
coloca.o: coloca.cpp
	${CC} -c coloca.cpp ${CPPFLAGS}
TratarMatriz.o: TratarMatriz.cpp TratarMatriz.hpp
	${CC} -c TratarMatriz.cpp ${CPPFLAGS}
CrearMatrizEdificios.o: CrearMatrizEdificios.cpp CrearMatrizEdificios.hpp
	${CC} -c CrearMatrizEdificios.cpp ${CPPFLAGS}

# Linkado
coloca: clean CrearMatrizEdificios.o TratarMatriz.o coloca.o
	${CC} coloca.o TratarMatriz.o CrearMatrizEdificios.o -o coloca

# LIMPIEZA de todo lo generado. Ojo: se borrarán sin preguntar
clean:
	${RM} CrearMatrizEdificios.o TratarMatriz.o coloca.o coloca
