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
    std::vector<Move> legal_moves = board->GetLegalMoves();

    for (Move m : legal_moves) {
        if (m) {
            chosen_move = 1;
            break;
        }

        chosen_move = NULL_MOVE;
    }

    if (chosen_move == NULL_MOVE) {
        printf("You won!");
        return;
    }

    while (true) {
        std::uniform_int_distribution<int> irando(0, legal_moves.size() - 1);
        int chosen = irando(rng);

        if (legal_moves[chosen]) {
            chosen_move = legal_moves[chosen];
            break;
        }
    }

    board->ExecuteMove(chosen_move);
    board->EndTurn();
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