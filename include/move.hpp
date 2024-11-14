#pragma once

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
// 0011       -----------------
// 0100       capture
// 1000       promote to queen
// 1001       promote to bishop
// 1010       promote to knight
// 1011       promote to rook
//   * promotion + capture is 11XX

bool IsNormalMove (Move);
bool IsCapture    (Move);
bool IsPromotion  (Move);
bool IsKSCastle   (Move);
bool IsQSCastle   (Move);
int  GetStartPos  (Move);
int  GetFinalPos  (Move);
int  GetFlags     (Move);
