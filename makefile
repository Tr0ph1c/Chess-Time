LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image
FILES = ./src/board.cpp ./src/main.cpp

main: $(FILES)
	g++ -g -Wall -o chesstime -I./include/ -L./lib/ $(FILES) $(LIBS)