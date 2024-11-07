#pragma once

#include <iostream>

#define BL 8       // BOARD LENGTH

namespace Pieces {
enum Piece {
    UNKNOWN = -1,
    EMPTY = 0,
    WHITE = 0x0100,
    BLACK = 0x0200,
    KING = 1  ,
    QUEEN     ,
    BISHOP    ,
    KNIGHT    ,
    ROOK      ,
    PAWN      ,
    W_KING = KING | WHITE,
    W_QUEEN   ,
    W_BISHOP  ,
    W_KNIGHT  ,
    W_ROOK    ,
    W_PAWN    ,
    B_KING = KING | BLACK,
    B_QUEEN   ,
    B_BISHOP  ,
    B_KNIGHT  ,
    B_ROOK    ,
    B_PAWN
};

Piece CharToPiece (char c );
char  PieceToChar (Piece p);
bool  IsWhite     (Piece p);
bool  IsBlack     (Piece p);
Piece RawPiece    (Piece p);
};

class Board {
    public:
    Pieces::Piece squares[BL][BL] = {Pieces::EMPTY};
    std::pair<int, int> selected_square = {-1, -1};
    Pieces::Piece color_to_play = Pieces::WHITE;

    void PrintBoard ();
    void FillBoard (const char* FEN);
    void RestartBoard ();
    
    bool IsWhiteToPlay ();
    bool IsEnemyPiece (Pieces::Piece p);
    bool IsAllyPiece (Pieces::Piece p);
    void EndTurn ();
    
    void Click (int x, int y);
    bool IsSelected ();
    void Deselect ();
};