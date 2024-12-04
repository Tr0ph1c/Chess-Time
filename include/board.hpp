#pragma once

#include "piece.hpp"
#include "move.hpp"
#include <vector>
#include <string>

// Random access array with easy insertion to the front
// specifically made to handle storage of moves
class SizeArray {
    private:
    uint32_t* array;
    size_t size = 0;
    size_t capacity = 0;
    static const int MAX_SIZE = 256;

    public:
    SizeArray& operator=(const SizeArray) = delete;
    uint32_t operator[] (size_t);
    uint32_t At (size_t);
    
    SizeArray ();
    SizeArray (size_t);
    ~SizeArray ();

    size_t Size ();
    void AddValue (uint32_t);
    void AddRestrictedMove (Move m, SizeArray* path);
    // TODO:
    // Use the trim function after generating the moves
    // then check and document any performance changes.
    void Trim ();
    void Clear ();
    bool Empty ();
};

class GameTracker {

public:
    std::vector<Move> move_history;
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

struct Pin {
    int position;
    int direction;
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
    int distance_to_edge[64][8] = {{7, 0, 7, 0, 0, 0, 7, 0},{7, 0, 6, 1, 1, 0, 6, 0},{7, 0, 5, 2, 2, 0, 5, 0},{7, 0, 4, 3, 3, 0, 4, 0},{7, 0, 3, 4, 4, 0, 3, 0},{7, 0, 2, 5, 5, 0, 2, 0},{7, 0, 1, 6, 6, 0, 1, 0},{7, 0, 0, 7, 7, 0, 0, 0},{6, 1, 7, 0, 0, 1, 6, 0},{6, 1, 6, 1, 1, 1, 6, 1},{6, 1, 5, 2, 2, 1, 5, 1},{6, 1, 4, 3, 3, 1, 4, 1},{6, 1, 3, 4, 4, 1, 3, 1},{6, 1, 2, 5, 5, 1, 2, 1},{6, 1, 1, 6, 6, 1, 1, 1},{6, 1, 0, 7, 6, 0, 0, 1},{5, 2, 7, 0, 0, 2, 5, 0},{5, 2, 6, 1, 1, 2, 5, 1},{5, 2, 5, 2, 2, 2, 5, 2},{5, 2, 4, 3, 3, 2, 4, 2},{5, 2, 3, 4, 4, 2, 3, 2},{5, 2, 2, 5, 5, 2, 2, 2},{5, 2, 1, 6, 5, 1, 1, 2},{5, 2, 0, 7, 5, 0, 0, 2},{4, 3, 7, 0, 0, 3, 4, 0},{4, 3, 6, 1, 1, 3, 4, 1},{4, 3, 5, 2, 2, 3, 4, 2},{4, 3, 4, 3, 3, 3, 4, 3},{4, 3, 3, 4, 4, 3, 3, 3},{4, 3, 2, 5, 4, 2, 2, 3},{4, 3, 1, 6, 4, 1, 1, 3},
    {4, 3, 0, 7, 4, 0, 0, 3},{3, 4, 7, 0, 0, 4, 3, 0},{3, 4, 6, 1, 1, 4, 3, 1},{3, 4, 5, 2, 2, 4, 3, 2},{3, 4, 4, 3, 3, 4, 3, 3},{3, 4, 3, 4, 3, 3, 3, 4},{3, 4, 2, 5, 3, 2, 2, 4},{3, 4, 1, 6, 3, 1, 1, 4},{3, 4, 0, 7, 3, 0, 0, 4},{2, 5, 7, 0, 0, 5, 2, 0},{2, 5, 6, 1, 1, 5, 2, 1},{2, 5, 5, 2, 2, 5, 2, 2},{2, 5, 4, 3, 2, 4, 2, 3},{2, 5, 3, 4, 2, 3, 2, 4},{2, 5, 2, 5, 2, 2, 2, 5},{2, 5, 1, 6, 2, 1, 1, 5},{2, 5, 0, 7, 2, 0, 0, 5},{1, 6, 7, 0, 0, 6, 1, 0},{1, 6, 6, 1, 1, 6, 1, 1},{1, 6, 5, 2, 1, 5, 1, 2},{1, 6, 4, 3, 1, 4, 1, 3},{1, 6, 3, 4, 1, 3, 1, 4},{1, 6, 2, 5, 1, 2, 1, 5},{1, 6, 1, 6, 1, 1, 1, 6},{1, 6, 0, 7, 1, 0, 0, 6},{0, 7, 7, 0, 0, 7, 0, 0},{0, 7, 6, 1, 0, 6, 0, 1},{0, 7, 5, 2, 0, 5, 0, 2},{0, 7, 4, 3, 0, 4, 0, 3},{0, 7, 3, 4, 0, 3, 0, 4},{0, 7, 2, 5, 0, 2, 0, 5},{0, 7, 1, 6, 0, 1, 0, 6},{0, 7, 0, 7, 0, 0, 0, 7},};

    uint8_t castling_rights = 0; // KQkq
    int enpassant_place = -1;
    Piece color_to_play = WHITE;
    int half_moves = 0;
    int fifty_move_rule = 0;
    uint8_t white_king_pos = 4;
    uint8_t black_king_pos = 60;
    // i think theoretically the maximum number of squares
    // that could be in this array at any given position
    // is nothing more than 8, but i made it 10 just as
    // a safety margin because i'm not entirely sure.
    SizeArray check_path = SizeArray(10);
    std::vector<Pin> pins = std::vector<Pin>(8);
    bool is_double_checked = false;

    void PrintBoard ();
    void LoadBoard (const char* FEN);
    void RestartBoard(const char* position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    inline bool IsWhiteToPlay ()                  { return color_to_play == WHITE; };
    inline bool IsEnemyPiece (Piece p)            { return p & (SwitchColor(color_to_play)); }
    inline bool IsAllyPiece  (Piece p)            { return p & color_to_play; }
    inline bool IsOfColor    (Piece p, Piece col) { return p & col; }
    inline bool EnPassantExists ()                { return enpassant_place != -1; }

    //void EndTurn ();

    void GetAllMoves (SizeArray*);
    void GetCheckPathAndPins ();

    bool IsInCheck ();
    bool IsSquareAttacked (int square_index);
    bool MoveWillExposeKing (Move); // Only used for en passant

    void ExecuteMove    (Move move, bool add_to_move_history = true              );
    void UndoMove       (Move move, bool forever = true, Move past_move = (Move)0);


    static int FullMovesToHalfMoves (int, bool);
    static int HalfMovesToFullMoves (int);
    static int NotationToBoardIndex (int, char);
    static int NotationToBoardIndex (int, int);

    std::string ExportPGN();
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