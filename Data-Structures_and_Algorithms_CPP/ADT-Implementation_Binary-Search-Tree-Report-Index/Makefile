CC = g++
CFLAGS = -Wall -Wextra -std=c++11
SRC = $(wildcard *.c) $(wildcard *.cpp)
OBJ = $(filter-out practica4.o, $(SRC:.c=.o))
OBJ := $(filter-out practica4.o, $(OBJ:.cpp=.o))
EXEC = practica4

all: $(EXEC)

$(EXEC): $(filter-out main_Practicas_1_y_2.o, $(OBJ)) practica4.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) practica4.o $(EXEC)

.PHONY: all clean