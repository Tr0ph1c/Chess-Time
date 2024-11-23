LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image
FILES = ./src/GUI.cpp ./src/piece.cpp ./src/move.cpp ./src/board.cpp ./src/main.cpp

all: $(FILES)
	g++ -g -Wall -o chesstime -I./include/ -L./lib/ $(FILES) $(LIBS)

run: all
	chesstime