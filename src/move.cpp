#include <iostream>
#include <string>
#include <sstream>
#include "move.hpp"
#include "piece.hpp"

//I use this function for debugging
std::string bin (Move m) {
    int datasz[] =
    { 3    ,3         ,4        ,4      ,6       , 6  } ;
    //   000       000           0000          0000       000000       000000
    //  moved    captured    castle rights     flags     start_pos    final_pos
    int *szptr = &datasz[5];


    char strnum[39] , *strptr = &strnum[37];
    strnum[38] = 0;

    Move x = 1;
    int sz_counter = 0;
    while (x){
        *strptr = (m & x)?'1':'0';
        strptr--;
        sz_counter++;
        x = x << 1;
        if (*szptr == sz_counter) {
            *strptr = ' ';
            sz_counter = 0;
            szptr--;
            strptr--;
        }

    }

    std::string final(strnum);
    return final;
}

Move CreateMove (uint32_t _start_pos, uint32_t _final_pos, uint32_t _flags, uint32_t _castle_rights, uint32_t _moved, uint32_t _captured) {
    return (_final_pos) | (_start_pos << 6) | (_flags << 12) | (_castle_rights << 16) | (_captured << 20) | (_moved << 23);
}

bool IsNormalMove (uint8_t mf) {
    return !mf;
}

bool IsCapture (uint8_t mf) {
    return mf & CAPTURE_MOVE;
}

bool IsEnPassant (uint8_t mf) {
    return mf == EN_PASSANT;
}

bool IsPromotion (uint8_t mf) {
    return mf & 0b1000;
}

bool IsKSCastle (uint8_t mf) {
    return mf == CASTLE_KS;
}

bool IsQSCastle (uint8_t mf) {
    return mf == CASTLE_QS;
}

bool IsDoublePawn (uint8_t mf) {
    return mf == DOUBLE_PAWN;
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
    return (m >> 20) & 0x7;
}
Piece GetMovedPieceFromMove (Move m) {
    return (m >> 23) & 0x7 ;
}

std::string GetNotationFromMove (Move m) {
    std::string s = "";
    return  s + GetPosFile(GetStartPos(m)) + GetPosRank(GetStartPos(m)) + GetPosFile(GetFinalPos(m)) + GetPosRank(GetFinalPos(m));
}

char GetPosFile(uint32_t pos) {
    return (char)((pos % 8) + 'a');
}

char GetPosRank(uint32_t pos) {
    return (char)(pos / 8 + '1');
}

std::string GetPGNFromMove (Move m) {
    std::stringstream ss;
    Piece p = GetMovedPieceFromMove(m);
    uint8_t flags = GetFlags(m);
    char piece_notation = PieceToChar(p | WHITE);
    uint32_t start = GetStartPos(m) , final = GetFinalPos(m);
    if (IsKSCastle(flags)){
        ss << "O-O";
    }
    else if(IsQSCastle(flags)){
        ss << "O-O-O";
    }
    else{
        if (p != PAWN) {
            ss << piece_notation << GetPosFile(start) << GetPosRank(start);
            if (IsCapture(flags)) { // piece takes
                ss << 'x';
            }
            ss << GetPosFile(final) << GetPosRank(final);
        } else {
            if (IsCapture(flags)) { // pawn takes
                ss << GetPosFile(start) << 'x';
            }
            ss << GetPosFile(final) << GetPosRank(final);
            if (IsPromotion(flags)) {
                ss << '=' << PieceToChar(GetPromotionPieceFromMove(m));
            }
        }
    }
    return ss.str();

}
