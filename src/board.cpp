#include "board.h"
#include <iostream>
#include <unordered_map>

const char* START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// Piece

Pieces::Piece Pieces::CharToPiece (char c) {
    switch (c) {
        case 'K':
            return Pieces::W_KING;
        case 'Q':
            return Pieces::W_QUEEN;
        case 'B':
            return Pieces::W_BISHOP;
        case 'N':
            return Pieces::W_KNIGHT;
        case 'R':
            return Pieces::W_ROOK;
        case 'P':
            return Pieces::W_PAWN;
        case 'k':
            return Pieces::B_KING;
        case 'q':
            return Pieces::B_QUEEN;
        case 'b':
            return Pieces::B_BISHOP;
        case 'n':
            return Pieces::B_KNIGHT;
        case 'r':
            return Pieces::B_ROOK;
        case 'p':
            return Pieces::B_PAWN;
        default:
            return Pieces::UNKNOWN;
    };
}

char Pieces::PieceToChar (Pieces::Piece p) {
    switch (p) {
        case Pieces::W_KING:
            return 'K';
        case Pieces::W_QUEEN:
            return 'Q';
        case Pieces::W_BISHOP:
            return 'B';
        case Pieces::W_KNIGHT:
            return 'N';
        case Pieces::W_ROOK:
            return 'R';
        case Pieces::W_PAWN:
            return 'P';
        case Pieces::B_KING:
            return 'k';
        case Pieces::B_QUEEN:
            return 'q';
        case Pieces::B_BISHOP:
            return 'b';
        case Pieces::B_KNIGHT:
            return 'n';
        case Pieces::B_ROOK:
            return 'r';
        case Pieces::B_PAWN:
            return 'p';
        default:
            return '?';
    };
}

bool Pieces::IsWhite (Pieces::Piece p) {
    return Pieces::WHITE & p;
}

bool Pieces::IsBlack (Pieces::Piece p) {
    return Pieces::BLACK & p;
}

Pieces::Piece Pieces::RawPiece (Pieces::Piece p) {
    return static_cast<Pieces::Piece>(0x00FF & p);
}

// Board

void Board::PrintBoard () {
    for (int rank = 0; rank < BL; ++rank) {
        for (int file = 0; file < BL; ++file) {
            std::cout << '[' << Pieces::PieceToChar(squares[rank][file]) << ']';
        }
        std::cout << std::endl;
    }
}

void Board::FillBoard (const char* FEN) {
    int rank = 0;
    int file = 0;

    while (char c = *FEN++) {
        if (c == ' ') break; // TEMPORARY SOLUTION

        if (c == '/') {
            rank++;
            file = 0;
        } else if (isdigit(c)) {
            file += c - '0'; // from char to int
        } else {
            squares[rank][file] = Pieces::CharToPiece(c);
            file++;
        }
    }
}

void Board::RestartBoard () {
    FillBoard(START_POS);
}