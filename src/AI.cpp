#include <iostream>

#include "AI.hpp"

AI::AI (Board* _board) {
    board = _board;
}

Move AI::CalculateMove () {
    SizeArray legal_moves;
    chosen_move = 0;
    board->GetAllMoves(&legal_moves);

    if (legal_moves.Empty()) {
        printf("%s\n", board->IsInCheck()? "You won!" : "Draw by stalemate.");
        return chosen_move;
    }

    evaluated_positions = 0;

    Search(search_depth, -AI_INFINITY, AI_INFINITY);

    printf("# of evaluated positions: %d\n", evaluated_positions);

    // When the search has to choose between moves that are
    // evaluated as -INFINITY it just doesn't choose a move
    // This is a dirty fix for that case.
    if (chosen_move == 0) {
        chosen_move = legal_moves[0];
    }

    return chosen_move;
}

int AI::Search (int depth, int alpha, int beta) {
    if (depth == 0) {
        // Quiescence search doesn't work well at all
        // without a transposition table
        //return QuiescentSearch(alpha, beta); causes crash too now lol
        return Evaluate();
    }

    SizeArray moves;
    board->GetAllMoves(&moves);

    // If no moves are available its checkmate
    // or stalemate depending on IsInCheck
    if (moves.Size() == 0) {
        if (board->IsInCheck()) {
            return -CHECKMATE - (depth) * 10; // a closer checkmate is a better checkmate
        }
        return 0;
    }

    int scores[moves.Size()] = {0};
    ScoreMoves(&moves, scores);

    for (size_t i = 0; i < moves.Size(); ++i) {
        PickMove(&moves, scores, i);
        Move curr_move = moves[i];

        board->ExecuteMove(curr_move);
        int next = -Search(depth - 1, -beta, -alpha);
        board->UndoMove(curr_move);

        if (next > alpha) {
            alpha = next;
            if (depth == search_depth) chosen_move = curr_move;
        }

        if (next >= beta) return beta;

        if (depth == search_depth) {
            printf("%s: %d - [%d]\n", GetNotationFromMove(curr_move).c_str(), next, scores[i]);
        }
    }

    return alpha;
}

void AI::ScoreMoves (SizeArray* moves, int* scores) {
    for (size_t i = 0; i < moves->Size(); ++i) {
        Move curr_move = moves->At(i);
        uint8_t flags = GetFlags(curr_move);

        if (IsCapture(flags)) {
            // offset by 900 to order all capture moves first
            scores[i] += 900 + (2 * RawPieceValue(GetCapturedPieceFromMove(curr_move)) - RawPieceValue(GetMovedPieceFromMove(curr_move)));
        }
    }
}

// This function works like a singular iteration of a selection sort
// we do it this way instead of a full selection sort to avoid losing time
// sorting bad moves since we could just prune them off the tree without sorting them.
// Keep in mind that each sorting iteration takes O(n) so it is a significant optimization.
void AI::PickMove(SizeArray* moves, int* scores, int start_index) {
    int n = moves->Size();

    for (int i = (start_index + 1); i < n - 1; ++i) {
        if (scores[i] > scores[start_index]) {
            std::swap(scores[i], scores[start_index]);

            Move tmp = moves->At(i);
            moves->ChangeAt(i, moves->At(start_index));
            moves->ChangeAt(start_index, tmp);
        }
    }
}

// TODO: Fix modifier problem (maybe split each heuristic into own function)
// Evaluates from the prespective of the playing color
int AI::Evaluate () {
    evaluated_positions++;

    int eval = 0;
    int piece_eval = 0;
    int pos_eval = 0;
    int modifier = board->IsWhiteToPlay()? 1 : -1;
    int endgame_value = 24;

    for (int i = 0; i < 64; ++i) {
        Piece p = board->squares[i];
        Piece raw = RawPiece(p);

        endgame_value -= game_phase_increment[raw];

        if (p != EMPTY) {
            piece_eval += PieceValue(p) * modifier;
            pos_eval += VALUE_OF_POSITION(p, i);
        }
    }

    float endgame_weight = endgame_value / 24.0f;
    
    if (evaluated_positions == 1) std::cout << "weight: " << endgame_weight << std::endl;
    
    eval += piece_eval;
    eval += pos_eval * (1 - endgame_weight); // positional play is less important in the endgame

    if (modifier == 1) {
        eval += ForceKingToCornerHeuristic(board->white_king_pos, board->black_king_pos, endgame_weight);
    } else {
        eval += ForceKingToCornerHeuristic(board->black_king_pos, board->white_king_pos, endgame_weight);
    }

    return eval;
}

int AI::ForceKingToCornerHeuristic (int friendly_king_square, int enemy_king_square, float endgame_weight) {
    int eval = 0;

    int friendly_rank = Rank(friendly_king_square);
    int friendly_file = File(friendly_king_square);
    int enemy_rank = Rank(enemy_king_square);
    int enemy_file = File(enemy_king_square);

    int enemy_distance_to_center = abs(enemy_rank - 3) + abs(enemy_file - 4);
    eval += enemy_distance_to_center;

    int distance_between_kings = abs(enemy_rank - friendly_rank) + abs(enemy_file - friendly_file);
    eval += 14 - distance_between_kings;

    return static_cast<int>(eval * 10 * endgame_weight);
}

int AI::QuiescentSearch (int alpha, int beta) {
    SizeArray moves;
    board->GetAllMoves(&moves);
    bool quiet = true;

    if (moves.Size() == 0) {
        if (board->IsInCheck()) {
            return -CHECKMATE;
        }
        return 0;
    }

    for (size_t i = 0; i < moves.Size(); ++i) {
        if (!IsCapture(moves[i])) continue; // IsCapture takes flags
        quiet = false;

        board->ExecuteMove(moves[i]);
        int next = -QuiescentSearch(-beta, -alpha);
        board->UndoMove(moves[i]);

        if (next >= beta) return beta;

        if (next > alpha) {
            alpha = next;
        }
    }

    if (quiet) return Evaluate();

    return alpha;
}