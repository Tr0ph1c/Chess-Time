#pragma once

#include "piece.hpp"
#include "move.hpp"
#include <vector>

class Board {
    public:
    Board ();
    
    const int directions[8] = {8, -8, 1, -1, 7, -7, 9, -9};
    const int knight_moves[8] = {15, 17, -15, -17, 6, 10, -6, -10};

    Piece squares[64] = {EMPTY};
    std::vector<Move> move_set;
    int distance_to_edge[64][8];
    int color_to_play = WHITE;
    int selected_square = -1;

    void PrintBoard ();
    void LoadBoard (const char* FEN);
    void RestartBoard ();
    
    bool IsWhiteToPlay ();
    bool IsEnemyPiece (int p);
    bool IsAllyPiece  (int p);
    void EndTurn ();

    void GeneratePawnMoves  (int ,int);
    void GenerateKingMoves  (int);
    void GenerateHorseMoves (int);
    void GenerateSlidingMoves (int, int);
    void GenerateMovesForSquare (int start_square);
    void GenerateAllMoves ();

    void ExecuteMove (Move);
    void HandlePawnMove(int, Move);

    static int NotationToBoardIndex (int, int);
    void PreCalculateDistancesToEdgeOfBoard ();
};

/*
 * a static array with an iterator interface
 * seems to be the best option for search speed
class MoveSet {
    Move moves[256] = {0};
    Move* head = 0;

    public:

};
*/