#pragma once

#include "piece.hpp"
#include "move.hpp"
#include <vector>
#include <string>

class GameTracker
{
private:
    std::vector<Move> move_history;

public:
    size_t MovesCount();
    bool IsEmpty();
    Move GetCurrMove();

    void ImportPGN(std::string);
    std::string ExportPGN();
    void PrintTracker ();

    void PushMove(Move mv);
    void UndoMove();
    void ResetTracker();
};

class Board {
    public:
    Board ();

    const char* START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const char* CUSTOM_POS = "rnbqkb1r/ppp2ppp/3p1n2/4p3/4PP2/2N5/PPPP2PP/R1BQKBNR w KQkq - 0 4";
    
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
    GameTracker tracker;
    std::vector<Move> move_set;
    int distance_to_edge[64][8];

    uint8_t castling_rights = 0b0000; // KQkq
    int enpassant_place = -1;
    Piece color_to_play = WHITE;
    int half_moves = 0;
    int fifty_move_rule = 0;
    uint8_t white_king_pos = 4;
    uint8_t black_king_pos = 60;

    void PrintBoard ();
    void LoadBoard (const char* FEN);
    void RestartBoard();
    
    inline bool IsWhiteToPlay () { return color_to_play == WHITE; };
    inline bool IsEnemyPiece (Piece p) { return p & (SwitchColor(color_to_play)); }
    inline bool IsAllyPiece (Piece p) { return p & color_to_play; }
    inline bool EnPassantExists () { return enpassant_place != -1; }

    void EndTurn ();

    std::vector<Move> GetAllMoves   ();
    std::vector<Move> GetLegalMoves ();

    bool IsInCheck ();
    bool IsSquareAttacked (int square_index);

    void ExecuteMove    (Move);
    void UndoMove       (Move);
    void HandlePawnMove (Piece, Move);

    static int FullMovesToHalfMoves (int, bool);
    static int HalfMovesToFullMoves (int);
    static int NotationToBoardIndex (int, char);
    static int NotationToBoardIndex (int, int);
    void PreCalculateDistancesToEdgeOfBoard ();
};

// old game tracker
// class GameTracker
// {
// private:
//     std::vector<Move> move_history;
//     //std::list<Move>::iterator curr_pos;
//     //uint8_t init_state_summary;
//     std::string start_pos; //FEN
//     //Board *board;

// public:
//     //GameTracker(Board *board);
//     //GameTracker(Board *board, std::string FEN);
//     //GameTracker();

//     size_t MovesCount();
//     bool IsEmpty();

//     void ImportPGN(std::string);
//     std::string ExportPGN();
//     void PrintTracker ();
//     /*
//     * NewMove Inset new move 
//     * Pre : This must be the last move played
//     */
//     void PushMove(Move mv);
//     void UndoMove();
//     void ResetTracker();
//     //void SetFEN(std::string  FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" );
//     //void SetFEN();

//     // // Moves stuff
//     // void Prev();
//     // void Next();
//     // /*
//     // * GoToMove change the board state to MoveNumber state
//     // * MoveNumber must be an intger value which (MoveNumber >= 0 && MoveNumber < MovesCount)
//     // */
//     // void GoToMove(int move_num);
//     // bool IsLastMove();
//     // bool IsFirstMove();
// };

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