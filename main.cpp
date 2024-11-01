#include <iostream>

#include "board.cpp"


Board board;


void Start () {
    board.RestartBoard();
    board.PrintBoard();
}

int main () {
    while (true) {
        char c;

        std::cout << "Input: ";
        std::cin >> c;

        switch (c) {
            case '0':
                exit(0);
            case '1':
                Start();
                break;
            case '2':
                std::cout << "FUCK EVERYBODY";
                break;
            default:
                std::cout << "invalid";
        }
    }
}