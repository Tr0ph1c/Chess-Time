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

Move CreateMove (uint8_t _start_pos, uint8_t _final_pos, uint8_t _flags, uint8_t _castle_rights, Piece _moved, Piece _captured) {
    return (_final_pos) | (_start_pos << 6) | (_flags << 12) | (_castle_rights << 16) | (_captured << 20) | (_moved << 23);
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
    return (m >> 20) & 0x7;
}
Piece GetMovedPieceFromMove (Move m) {
    return (m >> 23) & 0x7 ;
}

std::string GetNotationFromMove (Move m) {
    std::string s;
    return  s + (char)(GetStartPos(m) % 8 + 'a'+1) + std::to_string(GetStartPos(m) / 8 + 1) + (char)(GetFinalPos(m) % 8 + 'a') + std::to_string(GetFinalPos(m) / 8 + 1);
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
    char piece_notation = PieceToChar(p | WHITE);
    uint32_t start = GetStartPos(m) , final = GetFinalPos(m);
    if (IsKSCastle(m)){
        ss << "O-O";
    }
    else if(IsQSCastle(m)){
        ss << "O-O-O";
    }
    else{
        if (p != PAWN) {
            ss << piece_notation << GetPosFile(start) << GetPosRank(start);
            if (IsCapture(m)) { // piece takes
                ss << 'x';
            }
            ss << GetPosFile(final) << GetPosRank(final);
        } else {
            if (IsCapture(m)) { // pawn takes
                ss << GetPosFile(start) << 'x';
            }
            ss << GetPosFile(final) << GetPosRank(final);
            if (IsPromotion(m)) {
                ss << '=' << PieceToChar(GetPromotionPieceFromMove(m));
            }
        }
    }
    return ss.str();

}
