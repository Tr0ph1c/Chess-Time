INCLUDE = ./include/
FILES = ./src/board.cpp ./src/main.cpp

main: $(FILES)
	g++ $(FILES) -g -Wall -I$(INCLUDE) -L./lib/ -lSDL2 -o chesstime