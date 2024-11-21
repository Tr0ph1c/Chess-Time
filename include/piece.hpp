#pragma once
#include <iostream>
#include <cstdint>

typedef uint8_t Piece;

// 4-bit representation of a piece
// ===================================
//  00     000
// color  piece
//
// color : 01 = white, 10 = black
// piece : 0 = EMPTY, 1-6 = pieces

enum SQUARE_STATE
{
    COLOR_MASK = 0x18,
    PIECE_MASK = 0x7,
    SLIDE_MASK = 0x4,
    EMPTY = 0x0,
    WHITE = 0x8,
    BLACK = 0x10,
    KING = 0x1,
    KNIGHT,
    PAWN,
    QUEEN,
    BISHOP,
    ROOK,
    W_KING = KING | WHITE,
    W_KNIGHT,
    W_PAWN,
    W_QUEEN,
    W_BISHOP,
    W_ROOK,
    B_KING = KING | BLACK,
    B_KNIGHT,
    B_PAWN,
    B_QUEEN,
    B_BISHOP,
    B_ROOK
};

Piece   CharToPiece    (char c);
char    PieceToChar    (Piece p);
bool    IsWhite        (Piece p);
bool    IsBlack        (Piece p);
Piece   RawPiece       (Piece p);
bool    IsSlidingPiece (Piece p);
Piece   SwitchColor    (Piece p);