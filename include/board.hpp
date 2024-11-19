#pragma once

#include "piece.hpp"
#include "move.hpp"
#include "GameTracker.hpp"
#include <vector>

class Board {
    public:
    Board ();
    
    const int directions[8] = {8, -8, 1, -1, 7, -7, 9, -9};
    const int knight_moves[8] = {15, 17, -15, -17, 6, 10, -6, -10};
    const int white_kingside[2] = {5, 6};
    const int white_queenside[3] = {1, 2, 3};
    const int black_kingside[2] = {61, 62};
    const int black_queenside[3] = {57, 58, 59};
    const int WKSC_SQUARE = 6;
    const int WQSC_SQUARE = 2;
    const int BKSC_SQUARE = 62;
    const int BQSC_SQUARE = 58;

    Piece squares[64] = {EMPTY};
    std::vector<Move> move_set;
    int distance_to_edge[64][8];

    uint8_t castling_rights = 0b0000; // KQkq
    int color_to_play = WHITE;
    int half_moves = 0;
    int fifty_move_rule = 0;
    uint8_t white_king_pos;
    uint8_t black_king_pos;

    void PrintBoard ();
    void LoadBoard (const char* FEN);
    void RestartBoard(GameTracker *_tracker);
    
    inline bool IsWhiteToPlay ();
    inline bool IsEnemyPiece (int p);
    inline bool IsAllyPiece  (int p);
    void EndTurn ();

    void GeneratePawnMoves      (int ,int);
    void GenerateKingMoves      (int);
    void GenerateHorseMoves     (int);
    void GenerateSlidingMoves   (int, int);
    void GenerateMovesForSquare (int);
    void GenerateAllMoves       ();

    void ExecuteMove (Move);
    void HandlePawnMove(Piece, Move);

    static int FullMovesToHalfMoves (int, bool);
    static int HalfMovesToFullMoves (int);
    static int NotationToBoardIndex (int, char);
    static int NotationToBoardIndex (int, int);
    void PreCalculateDistancesToEdgeOfBoard ();

    static int ToDigit (char);
    static int GetNumberFromPointer (const char*);
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