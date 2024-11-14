#include <iostream>
#include "move.hpp"

bool IsNormalMove (Move m) {
    return !(GetFlags(m));
}

bool IsCapture (Move m) {
    return GetFlags(m) & 0b0100;
}

bool IsPromotion (Move m) {
    return GetFlags(m) & 0b1000;
}

bool IsKSCastle (Move m) {
    return GetFlags(m) == 0b0001;
}

bool IsQSCastle (Move m) {
    return GetFlags(m) == 0b0010;
}

int GetStartPos (Move m) {
    return (m >> 6) & 0x0FFF;
}

int GetFinalPos (Move m) {
    return m & 0x0FFF;
}

inline int GetFlags (Move m) {
    return (m >> 12);
}
