#include <iostream>

#include "SDL2/SDL.h"
#include "board.h"
#include "GUI.cpp"


Board board;
bool running = true;

void HandleEvents () {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;
        default:
            break;
    }
}

void Start () {
    board.RestartBoard();
    
    while (running) {
        HandleEvents();
        // take turns moving pieces
        GUI::GUI();
    }
}

int main (int argc, char** args) {
    GUI::Init();

    Start();

    GUI::Shutdown();
}