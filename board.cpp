#include <iostream>

#define BL 8       // BOARD LENGTH



const char* START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";



class Piece {
    public:
    char notation;
    bool white;
    //SDL_image image

    Piece (char _notation) {
        notation = _notation;
        white = isupper(notation);
    }
};

class Slot {
    Piece* piece;

    public:
    char PrintPiece () {
        return (piece != nullptr)? piece->notation : ' ';
    }

    void PlacePiece (Piece* p) {
        piece = p;
    }
};

class Board {
    Slot squares[BL][BL];

    public:
    void PrintBoard () {
        for (int rank = 0; rank < BL; ++rank) {
            for (int file = 0; file < BL; ++file) {
                std::cout << '[' << squares[rank][file].PrintPiece() << ']';
            }
            std::cout << std::endl;
        }
    }

    void FillBoard (const char* FEN) {
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

    void RestartBoard () {
        FillBoard(START_POS);
    }
};