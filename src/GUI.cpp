#include "SDL2/SDL.h"

#define WINDOW_W 800
#define WINDOW_H 800

namespace GUI {

SDL_Window*  window;
SDL_Renderer* renderer;

void GUI () {
    SDL_Delay(3000);
}

void Init () {
    if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1)) { 
        fprintf(stderr, "Could not initialize SDL: %s.\n", SDL_GetError());
        exit(-1);
    }

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow (
        "Chess Time",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_W, // W
        WINDOW_H, // H
        SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS
    );

    if (window == NULL) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        exit(-1);
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (renderer == NULL) {
        fprintf(stderr, "Could not create renderer: %s\n", SDL_GetError());
        exit(-1);
    }

    GUI();
}

void Shutdown () {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

}