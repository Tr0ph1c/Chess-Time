#pragma once

#include "piece.hpp"
#include <iostream>

#define PR_CAS 0xFF // Preserve castling rights

typedef uint32_t Move;

// 16-bit representation of a move:
// ===============================
//   000           0000          0000       000000       000000
// captured    castle rights     flags     start_pos    final_pos
//
// Castling Rights:
// KQkq
// K = King side white , Q = Queen side white
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
// 0011       double pawn push
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
    DOUBLE_PAWN  = 0b0011,
    CAPTURE_MOVE = 0b0100,
    EN_PASSANT   = 0b0111,
    PROMO_QUEEN  = 0b1000,
    PROMO_BISHOP = 0b1001,
    PROMO_KNIGHT = 0b1010,
    PROMO_ROOK   = 0b1011
};

Move CreateMove   (uint8_t _start, uint8_t _final, uint8_t _flags = QUIET_MOVE, uint8_t _castle_rights = PR_CAS, Piece _captured = EMPTY);

bool IsNormalMove (Move);
bool IsCapture    (Move);
bool IsEnPassant  (Move);
bool IsPromotion  (Move);
bool IsKSCastle   (Move);
bool IsQSCastle   (Move);
bool IsDoublePawn (Move);
int  GetStartPos  (Move);
int  GetFinalPos  (Move);
int  GetFlags     (Move);

Piece GetPromotionPieceFromMove (Move);