#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#define WINDOW_W 750
#define WINDOW_H 750

namespace GUI {

SDL_Window*  window;
SDL_Renderer* renderer;
SDL_Texture* pieces_png;

// board
SDL_Rect  BoardRect;
SDL_Color WhiteSquareColor = {.r = 254 , .g = 250 , .b = 241 ,.a = 255} ;
SDL_Color BlackSquareColor = {.r = 46 , .g = 7 , .b = 2 ,.a = 255} ;
TTF_Font *ConsolaFont;

void DrawPieceInside(SDL_Rect & dest ,int x ,int y){
    char text_buff[1024];
    sprintf(text_buff,"%c",'p');
    SDL_Surface* textSurface = TTF_RenderText_Solid( ConsolaFont, text_buff, (SDL_Color){0, 0, 255,255} );
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface( renderer, textSurface );
    SDL_FreeSurface(textSurface);
    // Get texture width and height
    // int textWidth, textHeight;

    SDL_RenderCopy( renderer, text_texture, NULL ,&dest);
    // printf("dist .x = % 4d , .y = % 4d , .w = % 4d ,h = % 4d \n" , dest.x,dest.y,dest.w,dest.h);

}

void DrawChessSquare(SDL_Color Color , int x /*From 0 to 7*/, int y /*From 0 to 7*/){
    SDL_Rect SquareRect;
    SquareRect.w = BoardRect.w / 8;
    SquareRect.h = BoardRect.h / 8;
    SquareRect.x = BoardRect.x + x * SquareRect.w;
    SquareRect.y = BoardRect.y + y * SquareRect.h;

    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_SetRenderDrawColor(renderer, Color.r, Color.g, Color.b, Color.a);
    SDL_RenderFillRect(renderer, &SquareRect);
    DrawPieceInside(SquareRect , x,y);

}

void RenderBoard(){
    bool IsSquareWhite = true;

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            DrawChessSquare((IsSquareWhite)? WhiteSquareColor:BlackSquareColor , j , i);
            IsSquareWhite = !IsSquareWhite;
        }
        IsSquareWhite = !IsSquareWhite;
        
    }
    

}

void GUI () {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    RenderBoard();
    SDL_RenderPresent(renderer);
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

    if ( TTF_Init() < 0 ) {
        fprintf(stderr, "Error initializing SDL_ttf: %s\n", TTF_GetError());
        exit(-1);
    }

    ConsolaFont = TTF_OpenFont("assets/fonts/ConsolaMono-Book.ttf",70);

    if ( !ConsolaFont ) {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        exit(-1);
    }

    //board
    BoardRect.w = WINDOW_W;
    BoardRect.h = WINDOW_H;
    BoardRect.x = 0;
    BoardRect.y = 0;
    // squares = _squares;

    

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
    // TTF_CloseFont( ConsolaFont ); // segmentation fault
    TTF_Quit();
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

}