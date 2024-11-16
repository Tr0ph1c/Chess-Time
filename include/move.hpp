#pragma once

#include "piece.hpp"
#include <iostream>

typedef uint16_t Move;

// 16-bit representation of a move:
// ===============================
// 0000       000000       000000
// flags     start_pos    final_pos
//
// Positions:
// storing a position takes 6 bits
// since a board index can be represented
// by numbers 0-63... 0b000000 - 0b111111
//
// Flags:
//   * The first bit is the promotion bit
//   * The second bit is the capture bit
// 0000       normal move
// 0001       castle king side
// 0010       castle queen side
// 0100       capture
// 0111       en passant
// 1000       promote to queen
// 1001       promote to bishop
// 1010       promote to knight
// 1011       promote to rook
//   * promotion + capture is 11XX

enum MOVE_FLAG {
    QUIET_MOVE   = 0,
    CASTLE_KS    = 0b0001,
    CASTLE_QS    = 0b0010,
    CAPTURE_MOVE = 0b0100,
    EN_PASSANT   = 0b0111,
    PROMO_QUEEN  = 0b1000,
    PROMO_BISHOP = 0b1001,
    PROMO_KNIGHT = 0b1010,
    PROMO_ROOK   = 0b1011
};

Move CreateMove   (uint8_t _start, uint8_t _final, uint8_t _flags);

bool IsNormalMove (Move);
bool IsCapture    (Move);
bool IsEnPassant  (Move);
bool IsPromotion  (Move);
bool IsKSCastle   (Move);
bool IsQSCastle   (Move);
int  GetStartPos  (Move);
int  GetFinalPos  (Move);
int  GetFlags     (Move);

Piece GetPromotionPieceFromMove (Move);