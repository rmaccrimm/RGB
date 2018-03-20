EXC = gb_emulator
CC = g++
CPPFLAGS = -Iinc
CFLAGS = -std=c++11 -Wall -g

SRC_DIR = src/
BIN_DIR = bin/
OBJ_DIR = obj/

SRC = $(wildcard $(SRC_DIR)*.cpp)
OBJ = $(SRC:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)

.PHONY: all debug clean

all: $(BIN_DIR)$(EXC)

debug: CPPFLAGS += -DDEBUG_MODE
debug: $(BIN_DIR)$(EXC)

$(BIN_DIR)$(EXC): $(OBJ)
	$(CC) $^ -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)
