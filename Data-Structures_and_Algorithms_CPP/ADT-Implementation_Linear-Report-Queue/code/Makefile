CC = g++
CFLAGS = -Wall -Wextra -std=c++11
SRC = $(wildcard *.c) $(wildcard *.cpp)
OBJ = $(filter-out programaPrueba.o, $(SRC:.c=.o))
OBJ := $(filter-out programaPrueba.o, $(OBJ:.cpp=.o))
EXEC = programaPrueba

all: $(EXEC)

$(EXEC): $(filter-out main_Practicas_1_y_2.o, $(OBJ)) programaPrueba.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) programaPrueba.o $(EXEC)

.PHONY: all clean