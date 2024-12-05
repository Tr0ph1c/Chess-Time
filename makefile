BIN = ./bin/
CC = g++
CFLAGS = -g -Wall
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image
FILES = $(BIN)AI.o $(BIN)GUI.o $(BIN)piece.o $(BIN)move.o $(BIN)board.o $(BIN)main.o

all: $(FILES) bin
	$(CC) $(CFLAGS)  -o $(BIN)chesstime -I./include/ -L./lib/ $(FILES) $(LIBS)

$(BIN)AI.o: ./src/AI.cpp bin
	$(CC) $(CFLAGS) -I./include/ -L./lib/ ./src/AI.cpp -c -o $(BIN)AI.o

$(BIN)GUI.o: ./src/GUI.cpp bin
	$(CC) $(CFLAGS) -I./include/ -L./lib/ ./src/GUI.cpp -c -o $(BIN)GUI.o

$(BIN)piece.o: ./src/piece.cpp bin
	$(CC) $(CFLAGS) -I./include/ -L./lib/ ./src/piece.cpp -c -o $(BIN)piece.o

$(BIN)move.o: ./src/move.cpp bin
	$(CC) $(CFLAGS) -I./include/ -L./lib/ ./src/move.cpp -c -o $(BIN)move.o

$(BIN)board.o: ./src/board.cpp bin
	$(CC) $(CFLAGS) -I./include/ -L./lib/ ./src/board.cpp -c -o $(BIN)board.o

$(BIN)main.o: ./src/main.cpp bin
	$(CC) $(CFLAGS) -I./include/ -L./lib/ ./src/main.cpp -c -o $(BIN)main.o

run: all
	$(BIN)chesstime

clean:
	del /S /Q bin

bin:
	mkdir bin
