#pragma once

#include <vector>

namespace Pieces {

// 8-bit representation of a piece
// ===================================
// 00    00      0       000
// --   color  mvflag   piece
//
// color : 01 = white, 10 = black
// mvflag: initial 0, when the piece moves it's set to 1
// piece : 0 = EMPTY, 1-6 = pieces, 7 = ERR_PIECE

enum Piece {
    ERR_PIECE = 0x0007,
    EMPTY = 0,
    WHITE = 0x0010,
    BLACK = 0x0020,
    KING = 1  ,
    QUEEN     ,
    BISHOP    ,
    KNIGHT    ,
    ROOK      ,
    PAWN      ,
    W_KING = KING | WHITE,
    W_QUEEN   ,
    W_BISHOP  ,
    W_KNIGHT  ,
    W_ROOK    ,
    W_PAWN    ,
    B_KING = KING | BLACK,
    B_QUEEN   ,
    B_BISHOP  ,
    B_KNIGHT  ,
    B_ROOK    ,
    B_PAWN
};

int   CharToPiece (char c);
char  PieceToChar (int p);
bool  IsWhite     (int p);
bool  IsBlack     (int p);
int   GetColor    (int p);
int   RawPiece    (int p);
bool  IsSlidingPiece (int p);
bool  HasMoved    (int p);
};

class Board {
    public:
    Board ();
    
    const int directions[8] = {8, -8, 1, -1, 7, -7, 9, -9};
    const int knight_moves[8] = {15, 17, -15, -17, 6, 10, -6, -10};

    int squares[64] = {Pieces::EMPTY};
    std::vector<int>* move_set[64];
    int distance_to_edge[64][8];
    int color_to_play = Pieces::WHITE;
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