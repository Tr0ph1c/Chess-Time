#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#define WINDOW_W 800
#define WINDOW_H 800

namespace GUI {

SDL_Window*  window;
SDL_Renderer* renderer;
SDL_Texture* pieces_png;

void GUI () {
    SDL_SetRenderDrawColor(renderer, 20, 50, 20, 255);

    SDL_RenderClear(renderer);
    
    SDL_RenderCopy(renderer, pieces_png, NULL, NULL);

    SDL_RenderPresent(renderer);
}

void InitTextures () {
    pieces_png = IMG_LoadTexture(renderer, "assets/pieces.png");
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

    if (IMG_Init(IMG_INIT_PNG) < 0) {
        fprintf(stderr, "Error initializing SDL_IMG: %s\n", IMG_GetError());
        exit(-1);
    }

    pieces_png = IMG_LoadTexture(renderer, "src/assets/pieces.png");

    if (!pieces_png) {
        fprintf(stderr, "a7a yasta");
        exit(-1);
    }
    //InitTextures();
}

void Shutdown () {
    IMG_Quit();
    SDL_DestroyTexture(pieces_png);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

}