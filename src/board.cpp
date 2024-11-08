#include "board.h"
#include <iostream>

const char* START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const char* VIENNA_POS = "rnbqkb1r/ppp2ppp/3p1n2/4p3/4PP2/2N5/PPPP2PP/R1BQKBNR w KQkq - 0 4";

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
    for (int index = 0; index < 64; ++index) {
        if (!(index % 8)) std::cout << std::endl;
        std::cout << '[' << PieceToChar(squares[index]) << ']';
    }
}

void Board::FillBoard (const char* FEN) {
    size_t index = 0;

    while (char c = *FEN++) {
        if (c == ' ') break; // TEMPORARY SOLUTION

        if (c == '/') {
            continue;
        } else if (isdigit(c)) {
            index += c - '0'; // from char to int
        } else {
            squares[index++] = CharToPiece(c);
        }
    }
}

void Board::RestartBoard () {
    //FillBoard(START_POS);
    FillBoard(VIENNA_POS);
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
    int board_index = Board::NotationToBoardIndex(rank, file);
    Piece _selected = squares[board_index];

    if (IsSelected()) {
        if (true) {
            Piece selected = squares[selected_square];
            squares[selected_square] = EMPTY;
            squares[board_index] = selected;
            Deselect();
            EndTurn();
        } else {
            Deselect();
        }
    } else if (IsAllyPiece(_selected)) {
        selected_square = board_index;
    }
}

bool Board::IsSelected () {
    return selected_square != -1;
}

void Board::Deselect () {
    selected_square = -1;
}

int Board::NotationToBoardIndex (int rank, int file) {
    return rank * 8 + file;
}