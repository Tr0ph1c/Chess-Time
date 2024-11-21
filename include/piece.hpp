#pragma once
#include <iostream>
#include <cstdint>

// TODO:
// Refactor into 4 bits and change
// color representation to a boolean or int
// that goes from 1 to -1 to represent
// direction as well

typedef uint8_t Piece;

// 6-bit representation of a piece
// ===================================
//  00    0    000
// color  -   piece
//
// color : 01 = white, 10 = black
// piece : 0 = EMPTY, 1-6 = pieces

enum SQUARE_STATE {
    COLOR_MASK = 0x30,
    PIECE_MASK = 0x07,
    MOVED      = 0x08,
    EMPTY      = 0x00,
    WHITE      = 0x10,
    BLACK      = 0x20,
    KING       = 0x01,
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

Piece   CharToPiece    (char c);
char    PieceToChar    (Piece p);
bool    IsWhite        (Piece p);
bool    IsBlack        (Piece p);
Piece   GetColor       (Piece p);
Piece   RawPiece       (Piece p);
bool    IsSlidingPiece (Piece p);
bool    HasMoved       (Piece p);