CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
SOURCES = practica4.cpp eventos.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXEC = practica4.exe

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXEC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q $(OBJECTS) $(EXEC)