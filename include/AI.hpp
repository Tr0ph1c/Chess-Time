#pragma once

#include "board.hpp"
#include "move.hpp"
#include "piece.hpp"

class AI {
    public:
    Board* board;

    AI (Board*);

    void PlayMove ();
    float Search (int depth);
    float Evaluate ();
};