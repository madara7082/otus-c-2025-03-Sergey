TARGET = solution
CC = gcc

LIBS = 
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -g -O0
LDFLAGS =

PREF_SRC = ./src/
PREF_OBJ = ./obj/

SRC = $(wildcard $(PREF_SRC)*.c)
OBJ = $(patsubst $(PREF_SRC)%.c, $(PREF_OBJ)%.o, $(SRC))

$(TARGET) : $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $(TARGET) $(LIBS)

$(PREF_OBJ)%.o : $(PREF_SRC)%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(TARGET) $(PREF_OBJ)*.o
