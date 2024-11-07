#include "board.h"
#include <iostream>

const char* START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

using namespace Pieces;

// Piece

Piece Pieces::CharToPiece (char c) {
    switch (c) {
        case 'K':
            return W_KING;
        case 'Q':
            return W_QUEEN;
        case 'B':
            return W_BISHOP;
        case 'N':
            return W_KNIGHT;
        case 'R':
            return W_ROOK;
        case 'P':
            return W_PAWN;
        case 'k':
            return B_KING;
        case 'q':
            return B_QUEEN;
        case 'b':
            return B_BISHOP;
        case 'n':
            return B_KNIGHT;
        case 'r':
            return B_ROOK;
        case 'p':
            return B_PAWN;
        default:
            return UNKNOWN;
    };
}

char Pieces::PieceToChar (Piece p) {
    switch (p) {
        case W_KING:
            return 'K';
        case W_QUEEN:
            return 'Q';
        case W_BISHOP:
            return 'B';
        case W_KNIGHT:
            return 'N';
        case W_ROOK:
            return 'R';
        case W_PAWN:
            return 'P';
        case B_KING:
            return 'k';
        case B_QUEEN:
            return 'q';
        case B_BISHOP:
            return 'b';
        case B_KNIGHT:
            return 'n';
        case B_ROOK:
            return 'r';
        case B_PAWN:
            return 'p';
        default:
            return '?';
    };
}

bool Pieces::IsWhite (Piece p) {
    return WHITE & p;
}

bool Pieces::IsBlack (Piece p) {
    return BLACK & p;
}

Piece Pieces::RawPiece (Piece p) {
    return static_cast<Piece>(0x00FF & p);
}

// Board

void Board::PrintBoard () {
    for (int rank = 0; rank < BL; ++rank) {
        for (int file = 0; file < BL; ++file) {
            std::cout << '[' << PieceToChar(squares[rank][file]) << ']';
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
            squares[rank][file] = CharToPiece(c);
            file++;
        }
    }
}

void Board::RestartBoard () {
    FillBoard(START_POS);
}

bool Board::IsWhiteToPlay () {
    return color_to_play == WHITE;
}

bool Board::IsEnemyPiece (Piece p) {
    return !(p & color_to_play);
}

bool Board::IsAllyPiece (Piece p) {
    return p & color_to_play;
}

void Board::EndTurn () {
    // stop clock
    color_to_play = (color_to_play==WHITE)? BLACK : WHITE;
}

void Board::Click (int rank, int file) {
    Piece _selected = squares[rank][file];

    if (IsSelected()) {
        if (true /* ValidMove(rank, file) */) {
            Piece selected = squares[selected_square.first][selected_square.second];
            squares[selected_square.first][selected_square.second] = EMPTY;
            squares[rank][file] = selected;
            Deselect();
            EndTurn();
        } else {
            Deselect();
        }
    } else if (IsAllyPiece(_selected)) {
        selected_square = {rank, file};
    }
}

bool Board::IsSelected () {
    return selected_square.first != -1;
}

void Board::Deselect () {
    selected_square.first = -1;
}