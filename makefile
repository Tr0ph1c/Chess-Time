LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image
FILES = ./src/piece.cpp ./src/move.cpp ./src/board.cpp ./src/main.cpp

GameTracker.o: ./src/GameTracker.cpp ./include/GameTracker.hpp
	g++ -g -Wall -c ./src/GameTracker.cpp -o GameTracker.o -I./include/ -L./lib/ $(LIBS)

all: $(FILES) GameTracker.o
	g++ -g -Wall -o chesstime -I./include/ -L./lib/ $(FILES) GameTracker.o $(LIBS)

run: all
	chesstime

clean:
	rm *.o *.exe