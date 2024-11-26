#pragma once

#include "board.hpp"
#include "move.hpp"
#include "piece.hpp"
#include <limits>

class AI {
    const int INFINITY = std::numeric_limits<int>::max();
    const int queen_value = 900;
    const int rook_value = 500;
    const int bishop_value = 320;
    const int knight_value = 300;
    const int pawn_value = 100;

    public:
    Board* board;
    Move chosen_move;
    int search_depth = 5;

    AI (Board*);

    void PlayMove ();
    int Search (int depth, int alpha, int beta);
    int Evaluate ();
};