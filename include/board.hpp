#pragma once

#include <vector>
#include "piece.hpp"

class Board {
    public:
    Board ();
    
    const int directions[8] = {8, -8, 1, -1, 7, -7, 9, -9};
    const int knight_moves[8] = {15, 17, -15, -17, 6, 10, -6, -10};

    int squares[64] = {EMPTY};
    std::vector<int>* move_set[64];
    int distance_to_edge[64][8];
    int color_to_play = WHITE;
    int selected_square = -1;

    void PrintBoard ();
    void LoadBoard (const char* FEN);
    void RestartBoard ();
    void InitializeMoveset ();
    
    bool IsWhiteToPlay ();
    bool IsEnemyPiece (int p);
    bool IsAllyPiece  (int p);
    void EndTurn ();

    void GeneratePawnMoves  (int ,int, std::vector<int>*);
    void GenerateKingMoves  (int, std::vector<int>*);
    void GenerateHorseMoves (int, std::vector<int>*);
    void GenerateSlidingMoves (int, int, std::vector<int>*);
    void GenerateMovesForSquare (int start_square);
    void GenerateAllMoves ();
    
    void Click (int x, int y);
    bool IsSelected ();
    void Deselect ();
    void HandlePawnMove(int, int, int, int);

    static int NotationToBoardIndex (int, int);
    void PreCalculateDistancesToEdgeOfBoard ();
};