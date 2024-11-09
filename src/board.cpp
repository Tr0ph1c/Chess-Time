#include "board.h"
#include <iostream>
#include <algorithm>

const char* START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const char* VIENNA_POS = "rnbqkb1r/ppp2ppp/3p1n2/4p3/4PP2/2N5/PPPP2PP/R1BQKBNR w KQkq - 0 4";


struct {
    int IsAvailable = -1;
    int place;
} EnPassant;

int half_moves = 1;

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

bool Pieces::IsSlidingPiece (Piece p) {
    bool is_sliding = false;

    switch (RawPiece(p)) {
        case QUEEN:
        case BISHOP:
        case ROOK:
            is_sliding = true;
            break;
        default:
            is_sliding = false;
    }

    return is_sliding;
}

// Board

Board::Board () {
    PreCalculateDistancesToEdgeOfBoard();
}

void Board::PrintBoard () {
    /*
    // DOESN'T WORK AS INTENDED ANYMORE
    for (int index = 0; index < 64; ++index) {
        if (!(index % 8)) std::cout << std::endl;
        std::cout << '[' << PieceToChar(squares[index]) << ']';
    }
    */
}

void Board::LoadBoard (const char* FEN) {
    size_t index = 64 - 8;

    while (char c = *FEN++) {
        if (c == ' ') break;  // TEMPORARY SOLUTION

        if (c == '/') {
            index -= 16;      // jump back to previous rank
        } else if (isdigit(c)) {
            index += c - '0'; // from char to int (jump number of files)
        } else {
            squares[index++] = CharToPiece(c);
        }
    }
}

void Board::RestartBoard () {
    //FillBoard(START_POS);
    LoadBoard(VIENNA_POS);
    InitializeMoveset();
    GenerateAllMoves();
}

void Board::InitializeMoveset () {
    for (int i = 0; i < 64; ++i) {
        move_set[i] = new std::vector<int>();
    }
}

bool Board::IsWhiteToPlay () {
    return color_to_play == WHITE;
}

bool Board::IsEnemyPiece (Piece p) {
    return p & (color_to_play ^ 0x0300); // flips the bits of white and black
}

bool Board::IsAllyPiece (Piece p) { 
    return p & color_to_play;
}

void Board::EndTurn () {
    // stop clock
    half_moves++;
    color_to_play = (color_to_play == WHITE)? BLACK : WHITE;
    GenerateAllMoves();
}

void Board::GeneratePawnMoves (Piece p,int start_square, std::vector<int>* moves) {
    short rank = start_square / 8;
    bool is_first_move = (IsWhite(p))? rank == 1 : rank == 6;
    short dir = (IsWhite(p))? 1 : -1;

    if(squares[start_square + dir*8] == Piece::EMPTY) {
        moves->push_back(start_square + dir*8);
        if (is_first_move && squares[start_square + dir*16] == Piece::EMPTY)
            moves->push_back(start_square + dir*16);
    }

    int first_diagonal = start_square + dir*7;
    int second_diagonal = start_square + dir*9;
    if (IsEnemyPiece(squares[first_diagonal]) &&
    abs(start_square%8 - first_diagonal%8) == 1)
        moves->push_back(first_diagonal);
    if (IsEnemyPiece(squares[second_diagonal]) &&
    abs(start_square%8 - second_diagonal%8) == 1)
        moves->push_back(second_diagonal);

    // en passant
    if (half_moves == EnPassant.IsAvailable) {
        if (first_diagonal == EnPassant.place &&
        abs(start_square%8 - first_diagonal%8) == 1) {
            moves->push_back(first_diagonal);
        } else if(second_diagonal == EnPassant.place &&
        abs(start_square%8 - second_diagonal%8) == 1) {
            moves->push_back(second_diagonal);
        }
    }
}

void Board::GenerateKingMoves (int start_square, std::vector<int>* moves) {
    for (int dir = 0; dir < 8; ++dir) {
        int target_square = start_square + directions[dir];
        if (target_square < 0 || target_square > 63) continue;
        if (IsAllyPiece(squares[target_square])) continue;
        // if the square is attacked skip too
        moves->push_back(target_square);
    }
}

void Board::GenerateHorseMoves (int start_square, std::vector<int>* moves) {
    int start_file = start_square % 8;

    for (int move : knight_moves) {
        int next_square = start_square + move;
        int next_file = next_square % 8;

        if (next_square >= 0 && next_square < 64) { // Make sure its on the board
            if (abs(start_file - next_file) < 3) {  // Make sure no warping
                if (IsAllyPiece(squares[next_square])) continue;
                moves->push_back(next_square);
            }
        }
    }
}

void Board::GenerateSlidingMoves (Piece p, int start_square, std::vector<int>* moves) {
    //printf("Generated for %c on square %d\n", PieceToChar(p), start_square);
    int diri = 0;
    int diri_max = 8;

    Piece raw_p = RawPiece(p);

    if (raw_p == ROOK) diri_max = 4;
    else if (raw_p == BISHOP) diri = 4;
    
    for (; diri < diri_max; ++diri) {
        for (int i = 0; i < distance_to_edge[start_square][diri]; ++i) {
            int next_square = start_square + (directions[diri] * (i + 1));
            if (IsAllyPiece(squares[next_square])) break;
            moves->push_back(next_square);
            //printf("%d %d\n", next_square / 8, next_square % 8);
            if (IsEnemyPiece(squares[next_square])) break;
        }
    }
}

void Board::GenerateMovesForSquare (int start_square) {
    std::vector<int>* moves = move_set[start_square];
    moves->clear();

    Piece _piece = squares[start_square];
    Piece _raw   = RawPiece(_piece);

    if (IsAllyPiece(_piece)) {
        if (IsSlidingPiece(_piece)) {
            GenerateSlidingMoves(_piece, start_square, moves);
        } else if (_raw == KNIGHT) {
            GenerateHorseMoves(start_square, moves);
        } else if (_raw == KING) {
            GenerateKingMoves(start_square, moves);
        } else if (_raw == PAWN) {
            GeneratePawnMoves(_piece,start_square, moves);
        }
    }
}

void Board::GenerateAllMoves () {
    for (int i = 0; i < 64; ++i) {
        GenerateMovesForSquare(i);
    }
}

void Board::Click (int rank, int file) {
    int board_index = Board::NotationToBoardIndex(rank, file);
    Piece _selected = squares[board_index];

    if (IsSelected()) {
        if (IsAllyPiece(_selected)) {
            selected_square = board_index;
        } else if (std::find(move_set[selected_square]->begin(), move_set[selected_square]->end(), board_index) != move_set[selected_square]->end()) {
            Piece selected = squares[selected_square];
            squares[selected_square] = EMPTY;
            squares[board_index] = selected;
            if (RawPiece(selected) == Piece::PAWN)
                HandlePawnMove(selected, board_index, selected_square);
            
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

void Board::PreCalculateDistancesToEdgeOfBoard () {
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            int north = rank;
            int south = 7 - rank;
            int east = 7 - file;
            int west = file;

            int* dte = distance_to_edge[NotationToBoardIndex(rank, file)];

            dte[0] = south;
            dte[1] = north;
            dte[2] = east;
            dte[3] = west;
            dte[4] = std::min(south, west);
            dte[5] = std::min(north, east);
            dte[6] = std::min(south, east);
            dte[7] = std::min(north, west);
        }
    }
}

void Board::HandlePawnMove (Piece p, int NewPos, int OldPos) {
    int dir = (IsWhite(p))? 1 : -1;

    if (abs(NewPos/8 - OldPos/8) == 2) {
        EnPassant.IsAvailable = half_moves + 1;
        EnPassant.place = OldPos + dir*8;
        // std::cout <<"En Passant"<<std::endl; 
        // std::cout <<"IsAvailable,place "<<EnPassant.IsAvailable<<' '<<EnPassant.place<<std::endl; 
    }

    if (EnPassant.place == NewPos && EnPassant.IsAvailable == half_moves) {
        squares[NewPos + dir*-1 * 8] = Piece::EMPTY;
    }
}