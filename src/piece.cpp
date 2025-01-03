#include "piece.hpp"
#include <iostream>

Piece CharToPiece (char c) {
    switch (c) {
        case 'K':
            return W_KING;
        case 'Q':
            return W_QUEEN;
        case 'B':
            return W_BISHOP;
        case 'N':
            return W_KNIGHT;
        case 'R':
            return W_ROOK;
        case 'P':
            return W_PAWN;
        case 'k':
            return B_KING;
        case 'q':
            return B_QUEEN;
        case 'b':
            return B_BISHOP;
        case 'n':
            return B_KNIGHT;
        case 'r':
            return B_ROOK;
        case 'p':
            return B_PAWN;
        default:
            return PIECE_MASK;
    };
}

char PieceToChar (Piece p) {
    switch (p) {
        case W_KING:
            return 'K';
        case W_QUEEN:
            return 'Q';
        case W_BISHOP:
            return 'B';
        case W_KNIGHT:
            return 'N';
        case W_ROOK:
            return 'R';
        case W_PAWN:
            return 'P';
        case B_KING:
            return 'k';
        case B_QUEEN:
            return 'q';
        case B_BISHOP:
            return 'b';
        case B_KNIGHT:
            return 'n';
        case B_ROOK:
            return 'r';
        case B_PAWN:
            return 'p';
        default:
            return '?';
    };
}

bool IsWhite (Piece p) {
    return WHITE & p;
}

bool IsBlack (Piece p) {
    return BLACK & p;
}

Piece RawPiece (Piece p) {
    return p & PIECE_MASK;
}

bool IsSlidingPiece (Piece p) {
    return p & SLIDE_MASK;
}

Piece SwitchColor (Piece p) {
    return p ^ COLOR_MASK;
}