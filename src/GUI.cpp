#include "GUI.hpp"
// #include "Components.cpp" UNDER DEVELOPMENT

#include <iostream>
#include <algorithm>
#include <unordered_map>

GUI::GUI (Board* board) {
    Init(board);
    running = true;
}

GUI::~GUI () {
    Shutdown();
}

void GUI::DrawPieceInside (SDL_Rect & dest, int rank, int file) {
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

void GUI::DrawChessSquare (SDL_Color Color, int rank /*y-axis*/, int file /*x-axis*/) {
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

void GUI::RenderBoard () {
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

void GUI::FetchMoves () {
    board->GetLegalMoves(&move_set);
    FillHighlightMatrix();
}

void GUI::InitializeHighlightMatrix () {
    for (int i = 0; i < 64; ++i) {
        highlight_matrix[i] = new std::vector<PosMove>;
    }
}

void GUI::FreeHighlightMatrix () {
    for (int i = 0; i < 64; ++i) {
        delete highlight_matrix[i];
    }
}

void GUI::FillHighlightMatrix () {
    for (int i = 0; i < 64; ++i) {
        highlight_matrix[i]->clear();
    }

    for (size_t i = 0; i < move_set.Size(); ++i) {
        if (IsNullMove(move_set[i])) continue;

        int square_to_fill = GetStartPos(move_set[i]);
        PosMove pm(i, GetFinalPos(move_set[i]));
        highlight_matrix[square_to_fill]->push_back(pm);
        if (IsPromotion(move_set[i])) i += 3;
    }
}

bool GUI::IsSelected () {
    return selected_square != -1;
}

void GUI::Deselect () {
    selected_square = -1;
}

void GUI::Select (int square) {
    selected_square = square;
}

void GUI::ExecutePromotion (int promotion_offset) {
    if (is_user_promoting) {
        is_user_promoting = false;
        board->ExecuteMove(move_set.At(promotion_index + promotion_offset));
        FetchMoves();
        Deselect();
    }
}

void GUI::GiveUserPromotionChoice (int move_index) {
    is_user_promoting = true;
    promotion_index = move_index;
}

void GUI::UndoUserMove () {
    if (board->tracker.IsEmpty()) return;
    Deselect();
    board->UndoMove(board->tracker.GetCurrMove());
    FetchMoves();
}

void GUI::HandleBoardClick (int x, int y) {
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
                if (IsPromotion(move_set.At(move_to_execute->index))) {
                    GiveUserPromotionChoice(move_to_execute->index);
                    return;
                }
                board->ExecuteMove(move_set.At(move_to_execute->index));
                FetchMoves();
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

void GUI::Click (int x, int y) {

    if (is_user_promoting) return;

    if (    x < BoardRect.x ||
            x > BoardRect.x+BoardRect.w ||
            y < BoardRect.y ||
            y > BoardRect.y + BoardRect.h)
        return;

    HandleBoardClick(x,y);
}

void GUI::DrawArrow(int relevant_x ,int relevant_y ,bool is_to_right , SDL_Rect &parent) {
    SDL_Rect dest_rect;
    dest_rect.x = relevant_x + parent.x      ;
    dest_rect.y = relevant_y + parent.y      ;
    dest_rect.h = parent.h                   ;
    dest_rect.w = dest_rect.h*(194.0f/322.0f);

    (is_to_right)?
    SDL_RenderCopyEx(renderer, arrow_png, NULL, &dest_rect, 0.0, NULL, SDL_FLIP_NONE):
    SDL_RenderCopyEx(renderer, arrow_png, NULL, &dest_rect, 0.0, NULL, SDL_FLIP_HORIZONTAL);

}

void GUI::RenderBoardRightSide () {
    DrawArrow(0, 0, false, controls);
    if (is_user_promoting) SDL_RenderCopy(renderer, promo_tip_png, NULL, &promotion_tip);

//    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
//    SDL_RenderFillRect(renderer, &controlles);
}

void GUI::ShowFrame () {
    SDL_SetRenderDrawColor(renderer, BackgroundColor);
    SDL_RenderClear(renderer);
    RenderBoard();
    RenderBoardRightSide();
    SDL_RenderPresent(renderer);
}

void GUI::InitRects () {
    {
        BoardRect.w = std::min(WINDOW_W, WINDOW_H);
        BoardRect.h = BoardRect.w;
        BoardRect.x = WINDOW_W /2 - BoardRect.w/2;
        BoardRect.y = 0;
    }

    {
        promotion_tip.h = 200;
        promotion_tip.w = 200;
        promotion_tip.x = BoardRect.x + BoardRect.w + 50;
        promotion_tip.y = BoardRect.y - (promotion_tip.h / 2) + (BoardRect.h / 2);
    }

    {
        controls.x = BoardRect.x + BoardRect.w + 20;
        controls.y = BoardRect.y + 50;
        controls.w = 250;
        controls.h = 25;
    }
}

void GUI::InitTextures () {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    pieces_png = IMG_LoadTexture(renderer, "assets/imgs/pieces.png");
    arrow_png  = IMG_LoadTexture(renderer, "assets/imgs/arrow.png");
    promo_tip_png = IMG_LoadTexture(renderer, "assets/imgs/promo_tip.png");

    SDL_SetTextureScaleMode(pieces_png, SDL_ScaleModeBest);
    SDL_SetTextureScaleMode(arrow_png , SDL_ScaleModeBest);
    SDL_SetTextureScaleMode(promo_tip_png , SDL_ScaleModeBest);

    if (!pieces_png || !arrow_png || !promo_tip_png) {
        fprintf(stderr, "An image could not be loaded.\n");
        exit(-1);
    }
}

void GUI::Init (Board* _board) {
    board = _board;
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
    InitRects();

    //components
    // Components::arrow_png = arrow_png;
    // Components::renderer  = renderer;

    // Components::components.push(new Components::NextBtn(0, 0, &controls));
}

void GUI::Shutdown () {
    FreeHighlightMatrix();

    SDL_DestroyTexture(pieces_png);
    SDL_DestroyTexture(arrow_png);
    SDL_DestroyTexture(promo_tip_png);
    IMG_Quit();

    TTF_CloseFont(ConsolaFont);
    TTF_Quit();
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}