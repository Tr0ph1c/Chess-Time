#include <iostream>

#include "SDL2/SDL.h"
#include "board.h"
#include "GUI.cpp"

const int framerate = 30;
const int target_delta = 1000 / framerate;
Uint32 start_frame;
int frame_delta;

Board board;
bool running = true;

void HandleEvents () {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            GUI::Click(event.button.x, event.button.y);
            break;
        default:
            break;
    }
}

void Start () {
    board.RestartBoard();
    board.PrintBoard();
    
    while (running) {
        start_frame = SDL_GetTicks();

        HandleEvents();
        GUI::GUI();

        frame_delta = SDL_GetTicks();
        if (frame_delta < target_delta) SDL_Delay(target_delta - frame_delta);
    }
}

int main (int argc, char** args) {
    GUI::Init(&board);

    Start();

    GUI::Shutdown();

    return 0;
}