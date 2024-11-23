#include <iostream>
#include <string>
#include "move.hpp"

Move CreateMove (uint8_t _start_pos, uint8_t _final_pos, uint8_t _flags, uint8_t _castle_rights, Piece _captured) {
    return (((((((RawPiece(_captured) << 4) | _castle_rights) << 4) | _flags) << 6) | _start_pos) << 6) | _final_pos;
}

bool IsNullMove (Move m) {
    return !m;
}

bool IsNormalMove (Move m) {
    return !(GetFlags(m));
}

bool IsCapture (Move m) {
    return GetFlags(m) & CAPTURE_MOVE;
}

bool IsEnPassant (Move m) {
    return GetFlags(m) == EN_PASSANT;
}

bool IsPromotion (Move m) {
    return GetFlags(m) & 0b1000;
}

bool IsKSCastle (Move m) {
    return GetFlags(m) == CASTLE_KS;
}

bool IsQSCastle (Move m) {
    return GetFlags(m) == CASTLE_QS;
}

bool IsDoublePawn (Move m) {
    return GetFlags(m) == DOUBLE_PAWN;
}

Piece GetPromotionPieceFromMove (Move m) {
    switch (GetFlags(m) & 0b0011) {
        case 0:
            return QUEEN;
        case 1:
            return BISHOP;
        case 2:
            return KNIGHT;
        case 3:
            return ROOK;
        default:
            return PAWN;
    }
}

Piece GetCapturedPieceFromMove (Move m) {
    return (m >> 20);
}

std::string GetNotationFromMove (Move m) {
    std::string s;
    return  s + (char)(GetStartPos(m) % 8 + 'a') + std::to_string(GetStartPos(m) / 8 + 1) + (char)(GetFinalPos(m) % 8 + 'a') + std::to_string(GetFinalPos(m) / 8 + 1);
}
