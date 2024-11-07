#pragma once

#include <iostream>

#define BL 8       // BOARD LENGTH

class Piece {
    public:
    char notation;
    bool white;

    Piece (char _notation);
};

class Pawn : public Piece {
    // cool stuff
};

class Slot {
    Piece* piece;

    public:
    char PrintPiece ();
    void PlacePiece (Piece* p);
};

class Board {
    Slot squares[BL][BL];

    public:
    void PrintBoard ();

    void FillBoard (const char* FEN);

    void RestartBoard ();
};