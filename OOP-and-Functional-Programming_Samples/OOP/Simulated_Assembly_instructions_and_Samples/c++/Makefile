# Compilador y banderas de compilación
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11

# Lista de archivos objeto (con extensión .cc)
OBJS = main.o Programa.o add.o read.o write.o push.o dup.o jumpif.o mul.o swap.o over.o \
       ProgramaSuma.o ProgramaCuentaAtras.o ProgramaFactorial.o

# Nombre del ejecutable
TARGET = main

# Objetivo por defecto
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Regla para compilar main.cc
main.o: main.cc ProgramaSuma.h ProgramaCuentaAtras.h ProgramaFactorial.h
	$(CXX) $(CXXFLAGS) -c main.cc

# Compilación de Programa.cc
Programa.o: Programa.cc Programa.h Instruccion.h
	$(CXX) $(CXXFLAGS) -c Programa.cc

# Compilación de add.cc
add.o: add.cc add.h Instruccion.h Programa.h
	$(CXX) $(CXXFLAGS) -c add.cc

# Compilación de read.cc
read.o: read.cc read.h Instruccion.h Programa.h
	$(CXX) $(CXXFLAGS) -c read.cc

# Compilación de write.cc
write.o: write.cc write.h Instruccion.h Programa.h
	$(CXX) $(CXXFLAGS) -c write.cc

# Compilación de push.cc
push.o: push.cc push.h Instruccion.h Programa.h
	$(CXX) $(CXXFLAGS) -c push.cc

# Compilación de dup.cc
dup.o: dup.cc dup.h Instruccion.h Programa.h
	$(CXX) $(CXXFLAGS) -c dup.cc

# Compilación de jumpif.cc
jumpif.o: jumpif.cc jumpif.h Instruccion.h Programa.h
	$(CXX) $(CXXFLAGS) -c jumpif.cc

# Compilación de mul.cc
mul.o: mul.cc mul.h Instruccion.h Programa.h
	$(CXX) $(CXXFLAGS) -c mul.cc

# Compilación de swap.cc
swap.o: swap.cc swap.h Instruccion.h Programa.h
	$(CXX) $(CXXFLAGS) -c swap.cc

# Compilación de over.cc
over.o: over.cc over.h Instruccion.h Programa.h
	$(CXX) $(CXXFLAGS) -c over.cc

# Compilación de ProgramaSuma.cc
ProgramaSuma.o: ProgramaSuma.cc ProgramaSuma.h Programa.h Instruccion.h read.h add.h write.h
	$(CXX) $(CXXFLAGS) -c ProgramaSuma.cc

# Compilación de ProgramaCuentaAtras.cc
ProgramaCuentaAtras.o: ProgramaCuentaAtras.cc ProgramaCuentaAtras.h Programa.h Instruccion.h read.h dup.h write.h push.h add.h jumpif.h
	$(CXX) $(CXXFLAGS) -c ProgramaCuentaAtras.cc

# Compilación de ProgramaFactorial.cc
ProgramaFactorial.o: ProgramaFactorial.cc ProgramaFactorial.h Programa.h Instruccion.h push.h read.h swap.h over.h mul.h add.h dup.h jumpif.h write.h
	$(CXX) $(CXXFLAGS) -c ProgramaFactorial.cc

# Regla para limpiar archivos generados
clean:
	rm -f $(OBJS) $(TARGET)
