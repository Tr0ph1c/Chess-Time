#include <iostream>

#include "AI.hpp"
#include <random>
#include <chrono>

auto seed = static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count());
std::default_random_engine rng(seed);

AI::AI (Board* _board) {
    board = _board;
}

Move AI::PlayMove () {
    Move chosen_move;
    MoveArray legal_moves;
    board->GetAllMoves(&legal_moves);

    if (legal_moves.Empty()) {
        printf("You won!");
        return;
    }

    std::uniform_int_distribution<int> irando(0, legal_moves.Size() - 1);
    int chosen = irando(rng);

    if (legal_moves[chosen]) chosen_move = legal_moves[chosen];

    return chosen_move;
}

int AI::Search (int depth, int alpha, int beta) {
    // negamax + alphabeta pruning search up to fixed depth (recommended 4-5)
    return 0;
}

int AI::Evaluate () {
    // evaluate end positions of search and return evaluation
    // use heuristic functions
    return 0;
}