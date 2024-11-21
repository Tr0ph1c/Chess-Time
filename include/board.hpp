#pragma once

#include "piece.hpp"
#include "move.hpp"
#include <vector>
#include <list>
#include <string>

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
    void RestartBoard();
    
    inline bool IsWhiteToPlay () { return color_to_play == WHITE; };
    inline bool IsEnemyPiece (int p) { return p & (color_to_play ^ 0x0030); }
    inline bool IsAllyPiece (int p) { return p & color_to_play; }

    void EndTurn ();

    void GeneratePawnMoves      (int ,int);
    void GenerateKingMoves      (int);
    void GenerateHorseMoves     (int);
    void GenerateSlidingMoves   (int, int);
    void GenerateMovesForSquare (int);
    void GenerateAllMoves       ();

    void ExecuteMove    (Move);
    void UndoMove       (Move);
    void HandlePawnMove (Piece, Move);

    static int FullMovesToHalfMoves (int, bool);
    static int HalfMovesToFullMoves (int);
    static int NotationToBoardIndex (int, char);
    static int NotationToBoardIndex (int, int);
    void PreCalculateDistancesToEdgeOfBoard ();
};

class GameTracker
{
private:
    std::list<Move> Moves;
    std::list<Move>::iterator curr_pos;
    uint8_t init_state_summary;
    std::string start_pos; //FEN
    Board *board;

    void ResetTracker();

public:
    GameTracker(Board *board);
    GameTracker(Board *board, std::string FEN);
    GameTracker();

    size_t MovesCount();
    bool IsEmpty();

    void ImportPGN(std::string);
    std::string ExportPGN();
    void SetFEN(std::string  FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" );
    // void SetFEN();

    // Moves stuff
    void Prev();
    void Next();
    /*
    * GoToMove change the board state to MoveNumber state
    * MoveNumber must be an intger value which (MoveNumber >= 0 && MoveNumber < MovesCount)
    */
    void GoToMove(int move_num);
    bool IsThisLastMove();
    bool IsThisFristMove();
    /*
    * NewMove Inset new move 
    * Pre : This must be the last move played
    */
    void NewMove(Move mv);
    void UndoMove();
};

class HelperClass {
public:
static int ToDigit (char c) {
    if (c < '0' || c > '9') {
        printf("could not convert to digit: [%c]\n", c);
        return -1;
    }
    return static_cast<int>(c - '0');
}

static int GetNumberFromPointer (const char* p) {
    int result = 0;

    while (result < 100 && result > -1) {
        result *= 10;
        result += ToDigit(*p);
        if (*(p+1) == ' ' || *(p+1) == '\0') break;
        p++;
    }

    return result;
}
};