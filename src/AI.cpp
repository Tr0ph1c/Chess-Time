#include <iostream>

#include "AI.hpp"
#include <random>
#include <chrono>

auto seed = static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count());
std::default_random_engine rng(seed);

AI::AI (Board* _board) {
    board = _board;
}

void AI::PlayMove () {
    Move chosen_move;
    MoveArray legal_moves;
    board->GetLegalMoves(&legal_moves);

    if (legal_moves.NoLegalMoves()) {
        printf("You won!");
        return;
    }

    while (true) {
        std::uniform_int_distribution<int> irando(0, legal_moves.Size() - 1);
        int chosen = irando(rng);

        if (legal_moves[chosen]) {
            chosen_move = legal_moves[chosen];
            break;
        }
    }

    board->ExecuteMove(chosen_move);
}

float AI::Search (int depth) {
    // negamax + alphabeta pruning search up to fixed depth (recommended 4-5)
    return 0;
}

float AI::Evaluate () {
    // evaluate end positions of search and return evaluation
    // use heuristic functions
    return 0;
}