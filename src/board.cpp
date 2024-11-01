#include "board.h"

const char* START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// Piece

Piece::Piece (char _notation) {
    notation = _notation;
    white = isupper(notation);
}

// Slot

char Slot::PrintPiece () {
    return (piece != nullptr)? piece->notation : ' ';
}

void Slot::PlacePiece (Piece* p) {
    piece = p;
}


// Board

void Board::PrintBoard () {
    for (int rank = 0; rank < BL; ++rank) {
        for (int file = 0; file < BL; ++file) {
            std::cout << '[' << squares[rank][file].PrintPiece() << ']';
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
            squares[rank][file].PlacePiece(new Piece(c));
            file++;
        }
    }
}

void Board::RestartBoard () {
    FillBoard(START_POS);
}