EXC = gb_emulator
CC = g++
FLAGS = -std=c++11 -Wall

SRC_DIR = src/
INC_DIR = inc/
BIN_DIR = bin/
OBJ_DIR = obj/


#$(EXC): $(OBJS)
#	$(CC) -o $(EXC) $(OBJS) -I.

#$(EXC).o: gb.cpp 
#	$(CC) -c gb.cpp -I.

#$register8bit.o: register8bit.h register8bit.cpp
#	$(CC) -c register8bit.cpp -I.

#$register16bit.o: register16bit.h register16bit.cpp
#	$(CC) -c register16bit.cpp -I.

#$processor.o: processor.h processor.cpp
#	$(CC) -c processor.o -I.

clean:
	rm *.o 

