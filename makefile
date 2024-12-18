BIN = ./bin/
CC = g++
CFLAGS = -g -Wall
LIBS =
UNAME_S = $(shell uname -s)
ifeq ($(OS),Windows_NT)
	LIBS = -L./lib/ -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image
else
    ifeq ($(UNAME_S),Linux)
		LIBS = -lSDL2 -lSDL2_ttf -lSDL2_image
    endif
endif

FILES = $(BIN)AI.o $(BIN)GUI.o $(BIN)piece.o $(BIN)move.o $(BIN)board.o $(BIN)main.o

all: $(FILES) bin
	$(CC) $(CFLAGS)  -o $(BIN)chesstime -I./include/ $(FILES) $(LIBS)

$(BIN)AI.o: ./src/AI.cpp bin
	$(CC) $(CFLAGS) -I./include/ ./src/AI.cpp -c -o $(BIN)AI.o

$(BIN)GUI.o: ./src/GUI.cpp bin
	$(CC) $(CFLAGS) -I./include/ ./src/GUI.cpp -c -o $(BIN)GUI.o

$(BIN)piece.o: ./src/piece.cpp bin
	$(CC) $(CFLAGS) -I./include/ ./src/piece.cpp -c -o $(BIN)piece.o

$(BIN)move.o: ./src/move.cpp bin
	$(CC) $(CFLAGS) -I./include/ ./src/move.cpp -c -o $(BIN)move.o

$(BIN)board.o: ./src/board.cpp bin
	$(CC) $(CFLAGS) -I./include/ ./src/board.cpp -c -o $(BIN)board.o

$(BIN)main.o: ./src/main.cpp bin
	$(CC) $(CFLAGS) -I./include/ ./src/main.cpp -c -o $(BIN)main.o

run: all
	$(BIN)chesstime

clean:
ifeq ($(UNAME_S),Linux)
		rm -rf $(BIN)*
else
		del /S /Q bin
endif
	
bin:
	mkdir bin
