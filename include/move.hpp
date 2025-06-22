#pragma once

#include "piece.hpp"
#include <iostream>

#define Rank(X) X/8
#define File(X) X%8

typedef uint32_t Move;

// 23-bit representation of a move:
// ===============================
//   000       000           0000          0000       000000       000000
//  moved    captured    castle rights     flags     start_pos    final_pos
//
// Castling Rights:
// KQkq
// K = King side white , Q = Queen side white
// Represents the castling rights BEFORE the move was made
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

Move CreateMove (uint32_t _start, uint32_t _final, uint32_t _flags, uint32_t _castle_rights, uint32_t _moved, uint32_t _captured = EMPTY);

/*
Shorthands for analyzing flags
*/
bool IsNormalMove     (uint8_t flags);
bool IsCapture        (uint8_t flags);
bool IsEnPassant      (uint8_t flags);
bool IsPromotion      (uint8_t flags);
bool IsKSCastle       (uint8_t flags);
bool IsQSCastle       (uint8_t flags);
bool IsDoublePawn     (uint8_t flags);

inline uint32_t GetStartPos     (Move m) { return (m >> 6) & 0x3F; }
inline uint32_t GetFinalPos     (Move m) { return        m & 0x3F; }
inline uint8_t  GetFlags        (Move m) { return (m >> 12) & 0xF; }
inline int GetCastleRights      (Move m) { return (m >> 16) & 0xF; }

Piece GetPromotionPieceFromMove (Move);
Piece GetCapturedPieceFromMove  (Move); // returns raw piece
Piece GetMovedPieceFromMove     (Move); // returns raw piece
std::string GetNotationFromMove (Move);
std::string GetPGNFromMove      (Move);
// Takes board index and returns char for file
char GetPosFile (uint32_t);
// Takes board index and returns char for rank
char GetPosRank (uint32_t);