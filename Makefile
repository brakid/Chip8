CC=g++
CFLAGS=-g -Wall


SRC_DIR=./src
OUTPUT_NAME=chip8cpu.out

CPU_SRC=$(SRC_DIR)/main.cpp $(SRC_DIR)/cpu.cpp $(SRC_DIR)/display.cpp $(SRC_DIR)/keyboard.cpp $(SRC_DIR)/fileloader.cpp

OBJS=$(CPU_SRC:.cpp=.o)

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT_NAME) $(OBJS)

.cpp.o:
	$(CC) $(CFLAGS) -c $<  -o $@

clean:
	rm $(OUTPUT_NAME)
	rm $(SRC_DIR)/*.o
	rm $(SRC_DIR)/*.hpp.gch