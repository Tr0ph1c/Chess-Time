#include <iostream>

#include "board.h"
#include "GUI.cpp"


Board board;

void Start () {
    board.RestartBoard();
    board.PrintBoard();
}

int main (int argc, char** args) {
    GUI::Init();

    Start();

    GUI::Shutdown();
}