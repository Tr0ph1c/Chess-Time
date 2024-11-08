#include "board.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#include <iostream>

#define WINDOW_W 700
#define WINDOW_H 700

using namespace Pieces;

namespace GUI {

Board* board;

SDL_Window*  window;
SDL_Renderer* renderer;
SDL_Texture* pieces_png;

// board
SDL_Rect  BoardRect;
SDL_Color WhiteSquareColor = {.r = 255 , .g = 180 , .b = 100 ,.a = 255};
SDL_Color BlackSquareColor = {.r = 150 , .g = 100 , .b = 65 ,.a = 255};
SDL_Color HighlightColor = {.r = 255 , .g = 255 , .b = 0 ,.a = 90};
TTF_Font *ConsolaFont;

void DrawPieceInside (SDL_Rect & dest, int rank, int file) {
    SDL_Rect rect;
    
    Piece curr_piece = board->squares[Board::NotationToBoardIndex(rank, file)];
    if (!curr_piece) return;

    int offset_y = IsWhite(curr_piece)? 0 : 88;
    int offset_x = (RawPiece(curr_piece) - 1) * 88;

    rect.x = offset_x;
    rect.y = offset_y;
    rect.w = 87;
    rect.h = 87;

    SDL_RenderCopy(renderer, pieces_png, &rect, &dest);
}

void DrawChessSquare (SDL_Color Color, int rank /*y-axis*/, int file /*x-axis*/){
    SDL_Rect SquareRect;
    SquareRect.w = BoardRect.w / 8;
    SquareRect.h = BoardRect.h / 8;
    SquareRect.y = BoardRect.y + rank * SquareRect.h;
    SquareRect.x = BoardRect.x + file * SquareRect.w;

    SDL_SetRenderDrawColor(renderer, Color.r, Color.g, Color.b, Color.a);
    SDL_RenderFillRect(renderer, &SquareRect);

    if (Board::NotationToBoardIndex(rank, file) == board->selected_square) {
        SDL_SetRenderDrawColor(renderer, HighlightColor.r, HighlightColor.g, HighlightColor.b, HighlightColor.a);
        SDL_RenderFillRect(renderer, &SquareRect);
    }

    DrawPieceInside(SquareRect, rank, file);
}

void RenderBoard () {
    bool IsSquareWhite = true;

    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            DrawChessSquare((IsSquareWhite)? WhiteSquareColor:BlackSquareColor, rank, file);
            IsSquareWhite = !IsSquareWhite;
        }
        IsSquareWhite = !IsSquareWhite;
    }
}

void Click (int x, int y) {
    int rank = y / (WINDOW_H / 8);
    int file = x / (WINDOW_W / 8);

    board->Click(rank, file);
}

void GUI () {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    RenderBoard();
    SDL_RenderPresent(renderer);
}

void InitTextures () {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    pieces_png = IMG_LoadTexture(renderer, "assets/pieces.png");
    SDL_SetTextureScaleMode(pieces_png, SDL_ScaleModeBest);

    if (!pieces_png) {
        fprintf(stderr, "can not load image");
        exit(-1);
    }
}

void Init (Board* _board) {
    board = _board;

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
    BoardRect.w = std::min(WINDOW_W, WINDOW_H);
    BoardRect.h = BoardRect.w;
    BoardRect.x = 0;
    BoardRect.y = 0;

    if (renderer == NULL) {
        fprintf(stderr, "Could not create renderer: %s\n", SDL_GetError());
        exit(-1);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    if (IMG_Init(IMG_INIT_PNG) < 0) {
        fprintf(stderr, "Error initializing SDL_IMG: %s\n", IMG_GetError());
        exit(-1);
    }

    InitTextures();
}

void Shutdown () {
    IMG_Quit();
    SDL_DestroyTexture(pieces_png);
    TTF_CloseFont( ConsolaFont ); // segmentation fault
    TTF_Quit();
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

}