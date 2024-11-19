#include "board.hpp"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "Components.cpp"

#include <iostream>
#include <algorithm>
#include <unordered_map>

#define WINDOW_W 1400
#define WINDOW_H 700

#define BackgroundColor /*r*/22,/*g*/ 21,/*b*/18, /*a*/255

namespace GUI {

Board* board;
GameTracker *tracker;

SDL_Window*  window;
SDL_Renderer* renderer;
SDL_Texture* pieces_png , *arrow_png;

// board
SDL_Rect  BoardRect;
/* old
SDL_Color WhiteSquareColor = {.r = 255 , .g = 180 , .b = 100 ,.a = 255};
SDL_Color BlackSquareColor = {.r = 150 , .g = 100 , .b = 65 ,.a = 255};
*/
SDL_Color WhiteSquareColor = {.r = 240 , .g = 217 , .b = 181 ,.a = 255};
SDL_Color BlackSquareColor = {.r = 181 , .g = 136 , .b = 99 ,.a = 255};

SDL_Color HighlightColor = {.r = 255 , .g = 255 , .b = 0 ,.a = 90};
TTF_Font *ConsolaFont;


class PosMove {
    public:
    int position;
    int index;

    PosMove (int _ind, int _pos) {
        position = _pos;
        index = _ind;
    }

    bool operator == (const int _pm) {
        return _pm == position;
    }
};

std::vector<Move>* move_set = nullptr;
std::vector<PosMove>* highlight_matrix[64];
int selected_square = -1;

// TODO: make GUI.hpp
bool IsSelected();

void DrawPieceInside (SDL_Rect & dest, int rank, int file) {
    SDL_Rect rect;
    
    int curr_piece = board->squares[Board::NotationToBoardIndex(rank, file)];
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
    SquareRect.y = BoardRect.y + abs(rank - 7) * SquareRect.h;
    SquareRect.x = BoardRect.x + file * SquareRect.w;

    SDL_SetRenderDrawColor(renderer, Color.r, Color.g, Color.b, Color.a);
    SDL_RenderFillRect(renderer, &SquareRect);

    if (Board::NotationToBoardIndex(rank, file) == selected_square) {
        SDL_SetRenderDrawColor(renderer, HighlightColor.r, HighlightColor.g, HighlightColor.b, HighlightColor.a);
        SDL_RenderFillRect(renderer, &SquareRect);
    }

    DrawPieceInside(SquareRect, rank, file);
}

void RenderBoard () {
    bool IsSquareWhite = false;

    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            DrawChessSquare((IsSquareWhite)? WhiteSquareColor:BlackSquareColor, rank, file);
            IsSquareWhite = !IsSquareWhite;
        }
        IsSquareWhite = !IsSquareWhite;
    }

    if (IsSelected()) {
        for (PosMove m : *highlight_matrix[selected_square]) {
            DrawChessSquare(HighlightColor, m.position / 8, m.position % 8);
        }
    }
}

void InitializeHighlightMatrix () {
    for (int i = 0; i < 64; ++i) {
        highlight_matrix[i] = new std::vector<PosMove>;
    }
}

void FillHighlightMatrix () {
    for (int i = 0; i < 64; ++i) {
        highlight_matrix[i]->clear();
    }

    for (int i = 0; i < static_cast<int>(move_set->size()); ++i) {
        int square_to_fill = GetStartPos(move_set->at(i));
        PosMove pm(i, GetFinalPos(move_set->at(i)));
        highlight_matrix[square_to_fill]->push_back(pm);
    }
}

bool IsSelected () {
    return selected_square != -1;
}

void Deselect () {
    selected_square = -1;
}

void Select (int square) {
    selected_square = square;
}

void HandleBoardClick(int x, int y){
    int rank = abs(y - BoardRect.y - BoardRect.h) / (BoardRect.h / 8);
    int file = (x - BoardRect.x) / (BoardRect.w / 8);

    // board click
    int board_index = Board::NotationToBoardIndex(rank, file);
    int clicked = board->squares[board_index];

    if (IsSelected()) {
        if (board->IsAllyPiece(clicked)) {
            Select(board_index);
        } else {
            auto move_to_execute = std::find(highlight_matrix[selected_square]->begin(), highlight_matrix[selected_square]->end(), board_index);
            if (move_to_execute != highlight_matrix[selected_square]->end()) {
                board->ExecuteMove(move_set->at(move_to_execute->index));
                FillHighlightMatrix();
                Deselect();
            } else {
                Deselect();
                printf("move not possible\n");
            }
        }
    } else if (board->IsAllyPiece(clicked)) {
        Select(board_index);
    }

}

void Click (int x, int y) {
    if (    x < BoardRect.x ||
            x > BoardRect.x+BoardRect.w ||
            y < BoardRect.y ||
            y > BoardRect.y + BoardRect.h)
        return;

    HandleBoardClick(x,y);
}

void DrawArrow(int relevant_x ,int relevant_y ,bool is_to_right , SDL_Rect &parent){
    SDL_Rect dest_rect;
    dest_rect.x = relevant_x + parent.x      ;
    dest_rect.y = relevant_y + parent.y      ;
    dest_rect.h = parent.h                   ;
    dest_rect.w = dest_rect.h*(194.0f/322.0f);

    (is_to_right)?
    SDL_RenderCopyEx(renderer, arrow_png, NULL, &dest_rect, 0.0, NULL, SDL_FLIP_NONE):
    SDL_RenderCopyEx(renderer, arrow_png, NULL, &dest_rect, 0.0, NULL, SDL_FLIP_HORIZONTAL);

}

void RenderBoardRightSide(){
    SDL_Rect controlles;
    controlles.x = BoardRect.x + BoardRect.w + 20 ;
    controlles.y = BoardRect.y + 50               ;
    controlles.w = 250                            ;
    controlles.h = 25                             ;

//    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
//    SDL_RenderFillRect(renderer, &controlles);


    DrawArrow(0,0,false,controlles);
}

void GUI () {
    SDL_SetRenderDrawColor(renderer, BackgroundColor);
    SDL_RenderClear(renderer);
    RenderBoard();
    RenderBoardRightSide();
    SDL_RenderPresent(renderer);
}

void InitTextures () {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    pieces_png = IMG_LoadTexture(renderer, "assets/imgs/pieces.png");
    arrow_png  = IMG_LoadTexture(renderer, "assets/imgs/arrow.png") ;
    SDL_SetTextureScaleMode(pieces_png, SDL_ScaleModeBest);
    SDL_SetTextureScaleMode(arrow_png , SDL_ScaleModeBest);

    if (!pieces_png || ! arrow_png) {
        fprintf(stderr, "can not load image");
        exit(-1);
    }
}

void Init (Board* _board) {
    board = _board;
    move_set = &_board->move_set;
    InitializeHighlightMatrix();

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
    BoardRect.x = WINDOW_W /2 - BoardRect.w/2;
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

    //components
    Components::arrow_png = arrow_png;
    Components::tracker   = tracker;
    Components::renderer  = renderer;

    SDL_Rect controls;
    controls.x = BoardRect.x + BoardRect.w + 20 ;
    controls.y = BoardRect.y + 50               ;
    controls.w = 250                            ;
    controls.h = 25                             ;

    Components::components.push(new Components::NextBtn(0, 0, &controls));
}

void Shutdown () {
    IMG_Quit();
    SDL_DestroyTexture(pieces_png);
    SDL_DestroyTexture(arrow_png);
    TTF_CloseFont( ConsolaFont );
    TTF_Quit();
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

}