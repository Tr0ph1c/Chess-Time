BIN = ./bin/

LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image
FILES = $(BIN)AI.o $(BIN)GUI.o $(BIN)piece.o $(BIN)move.o $(BIN)board.o $(BIN)main.o

all: $(FILES)
	g++ -g -Wall -o $(BIN)chesstime -I./include/ -L./lib/ $(FILES) $(LIBS)

$(BIN)AI.o: ./src/AI.cpp
	g++ -I./include/ -L./lib/ ./src/AI.cpp -c -o $(BIN)AI.o

$(BIN)GUI.o: ./src/GUI.cpp
	g++ -I./include/ -L./lib/ ./src/GUI.cpp -c -o $(BIN)GUI.o

$(BIN)piece.o: ./src/piece.cpp
	g++ -I./include/ -L./lib/ ./src/piece.cpp -c -o $(BIN)piece.o

$(BIN)move.o: ./src/move.cpp
	g++ -I./include/ -L./lib/ ./src/move.cpp -c -o $(BIN)move.o

$(BIN)board.o: ./src/board.cpp
	g++ -I./include/ -L./lib/ ./src/board.cpp -c -o $(BIN)board.o

$(BIN)main.o: ./src/main.cpp
	g++ -I./include/ -L./lib/ ./src/main.cpp -c -o $(BIN)main.o

run: all
	$(BIN)chesstime