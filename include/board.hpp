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
    const char* START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const char* DEBUG_POS = "rnbqkbnr/pppppppp/8/8/8/1P6/P1PPPPPP/RNBQKBNR b KQkq - 0 1";
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
    // TODO: make the move_set into a normal array with a max size of 256
    // this will require lots of refactoring
    std::vector<Move> move_set;
    int distance_to_edge[64][8] = {
        {7, 0, 7, 0, 0, 0, 7, 0},
        {7, 0, 6, 1, 1, 0, 6, 0},
        {7, 0, 5, 2, 2, 0, 5, 0},
        {7, 0, 4, 3, 3, 0, 4, 0},
        {7, 0, 3, 4, 4, 0, 3, 0},
        {7, 0, 2, 5, 5, 0, 2, 0},
        {7, 0, 1, 6, 6, 0, 1, 0},
        {7, 0, 0, 7, 7, 0, 0, 0},
        {6, 1, 7, 0, 0, 1, 6, 0},
        {6, 1, 6, 1, 1, 1, 6, 1},
        {6, 1, 5, 2, 2, 1, 5, 1},
        {6, 1, 4, 3, 3, 1, 4, 1},
        {6, 1, 3, 4, 4, 1, 3, 1},
        {6, 1, 2, 5, 5, 1, 2, 1},
        {6, 1, 1, 6, 6, 1, 1, 1},
        {6, 1, 0, 7, 6, 0, 0, 1},
        {5, 2, 7, 0, 0, 2, 5, 0},
        {5, 2, 6, 1, 1, 2, 5, 1},
        {5, 2, 5, 2, 2, 2, 5, 2},
        {5, 2, 4, 3, 3, 2, 4, 2},
        {5, 2, 3, 4, 4, 2, 3, 2},
        {5, 2, 2, 5, 5, 2, 2, 2},
        {5, 2, 1, 6, 5, 1, 1, 2},
        {5, 2, 0, 7, 5, 0, 0, 2},
        {4, 3, 7, 0, 0, 3, 4, 0},
        {4, 3, 6, 1, 1, 3, 4, 1},
        {4, 3, 5, 2, 2, 3, 4, 2},
        {4, 3, 4, 3, 3, 3, 4, 3},
        {4, 3, 3, 4, 4, 3, 3, 3},
        {4, 3, 2, 5, 4, 2, 2, 3},
        {4, 3, 1, 6, 4, 1, 1, 3},
        {4, 3, 0, 7, 4, 0, 0, 3},
        {3, 4, 7, 0, 0, 4, 3, 0},
        {3, 4, 6, 1, 1, 4, 3, 1},
        {3, 4, 5, 2, 2, 4, 3, 2},
        {3, 4, 4, 3, 3, 4, 3, 3},
        {3, 4, 3, 4, 3, 3, 3, 4},
        {3, 4, 2, 5, 3, 2, 2, 4},
        {3, 4, 1, 6, 3, 1, 1, 4},
        {3, 4, 0, 7, 3, 0, 0, 4},
        {2, 5, 7, 0, 0, 5, 2, 0},
        {2, 5, 6, 1, 1, 5, 2, 1},
        {2, 5, 5, 2, 2, 5, 2, 2},
        {2, 5, 4, 3, 2, 4, 2, 3},
        {2, 5, 3, 4, 2, 3, 2, 4},
        {2, 5, 2, 5, 2, 2, 2, 5},
        {2, 5, 1, 6, 2, 1, 1, 5},
        {2, 5, 0, 7, 2, 0, 0, 5},
        {1, 6, 7, 0, 0, 6, 1, 0},
        {1, 6, 6, 1, 1, 6, 1, 1},
        {1, 6, 5, 2, 1, 5, 1, 2},
        {1, 6, 4, 3, 1, 4, 1, 3},
        {1, 6, 3, 4, 1, 3, 1, 4},
        {1, 6, 2, 5, 1, 2, 1, 5},
        {1, 6, 1, 6, 1, 1, 1, 6},
        {1, 6, 0, 7, 1, 0, 0, 6},
        {0, 7, 7, 0, 0, 7, 0, 0},
        {0, 7, 6, 1, 0, 6, 0, 1},
        {0, 7, 5, 2, 0, 5, 0, 2},
        {0, 7, 4, 3, 0, 4, 0, 3},
        {0, 7, 3, 4, 0, 3, 0, 4},
        {0, 7, 2, 5, 0, 2, 0, 5},
        {0, 7, 1, 6, 0, 1, 0, 6},
        {0, 7, 0, 7, 0, 0, 0, 7},
    };

    uint8_t castling_rights = 0b0000; // KQkq
    int enpassant_place = -1;
    Piece color_to_play = WHITE;
    int half_moves = 0;
    int fifty_move_rule = 0;
    uint8_t white_king_pos = 4;
    uint8_t black_king_pos = 60;

    void PrintBoard ();
    void LoadBoard (const char* FEN);
    void RestartBoard(const char* position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
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

    static int FullMovesToHalfMoves (int, bool);
    static int HalfMovesToFullMoves (int);
    static int NotationToBoardIndex (int, char);
    static int NotationToBoardIndex (int, int);
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