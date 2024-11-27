#pragma once

#include "board.hpp"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#define BOARD_W 700
#define BOARD_H 700

#define BackgroundColor /*r*/22, /*g*/21, /*b*/18, /*a*/255

class GUI {
public:
GUI (Board*);
~GUI ();

void RunGUITick ();
void FetchMoves ();
bool running = true;

private:
Board* board;

SDL_Window*  window;
SDL_Renderer* renderer;
int window_w;
int window_h;
int screen_w;
int screen_h;

// TEXTURES
SDL_Texture* pieces_png;
//SDL_Texture* arrow_png;
SDL_Texture* promo_tip_png;

// RECTS
SDL_Rect BoardRect;
SDL_Rect promotion_tip;
SDL_Rect controls;

// COLORS
SDL_Color WhiteSquareColor = {.r = 240 , .g = 217 , .b = 181 ,.a = 255};
SDL_Color BlackSquareColor = {.r = 181 , .g = 136 , .b = 99 ,.a = 255};
SDL_Color HighlightColor = {.r = 255 , .g = 255 , .b = 0 ,.a = 90};

// FONTS
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

MoveArray move_set;
std::vector<PosMove>* highlight_matrix[64];
int selected_square = -1;
bool is_user_promoting = false;
int promotion_index = 0;

SDL_Rect held_rect;

int target_delta;
Uint32 start_frame;
int frame_delta;

void Init (Board* _board);
void InitRects ();
void InitTextures ();

// void RenderBoardRightSide ();
// void DrawArrow (int relevant_x ,int relevant_y ,bool is_to_right , SDL_Rect &parent);
void DrawPieceInside (SDL_Rect & dest, int rank, int file);
void DrawChessSquare (SDL_Color Color, int rank, int file);
void RenderHeldPiece ();
void RenderBoard ();
void ShowFrame ();
void HandleEvents ();
void DrawFileNumber();
void DrawRankNumber();

void InitializeHighlightMatrix ();
void FreeHighlightMatrix ();
void FillHighlightMatrix ();

bool IsSelected ();
void Deselect ();
void Select (int square);

void ExecutePromotion (int promotion_offset);
void GiveUserPromotionChoice (int move_index);

void UndoUserMove ();
void HandleBoardClick (int x, int y);
void Click (int x, int y, bool mouse_down);

void Shutdown ();

};