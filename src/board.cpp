#include "board.hpp"
#include "piece.hpp"
#include "move.hpp"
#include <GameTracker.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

GameTracker *tracker;

const char* START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const char* VIENNA_POS = "rnbqkb1r/ppp2ppp/3p1n2/4p3/4PP2/2N5/PPPP2PP/R1BQKBNR w KQkq - 0 4";


struct {
    int IsAvailable = -1;
    int place;
} EnPassant;

int half_moves = 1;

// Board

Board::Board () {
    PreCalculateDistancesToEdgeOfBoard();
}

// TODO: convert current position to FEN string
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
    tracker = new GameTracker(this);
    // LoadBoard(VIENNA_POS);
    GenerateAllMoves();
}

bool Board::IsWhiteToPlay () {
    return color_to_play == WHITE;
}

bool Board::IsEnemyPiece (int p) {
    return p & (color_to_play ^ 0x0030); // flips the bits of white and black
}

bool Board::IsAllyPiece (int p) { 
    return p & color_to_play;
}

void Board::EndTurn () {
    // stop clock
    half_moves++;
    color_to_play = (color_to_play == WHITE)? BLACK : WHITE;
    GenerateAllMoves();
}

void Board::GeneratePawnMoves (int p, int start_square) {
    short dir = (IsWhite(p))? 1 : -1;
    short rank = start_square / 8;
    int forward_square = start_square + dir*8;

    bool is_first_move = (IsWhite(p))? rank == 1 : rank == 6;
    bool is_next_rank_promo = (forward_square / 8 == 0 || forward_square / 8 == 7);

    if(squares[forward_square] == EMPTY) {
        if (is_next_rank_promo) {
            move_set.push_back(CreateMove(start_square, forward_square, PROMO_QUEEN));
            move_set.push_back(CreateMove(start_square, forward_square, PROMO_BISHOP));
            move_set.push_back(CreateMove(start_square, forward_square, PROMO_KNIGHT));
            move_set.push_back(CreateMove(start_square, forward_square, PROMO_ROOK));
        } else {
            move_set.push_back(CreateMove(start_square, forward_square, QUIET_MOVE));
            if (is_first_move && squares[start_square + dir*16] == EMPTY)
                move_set.push_back(CreateMove(start_square, start_square + dir*16, QUIET_MOVE));
        }
    }

    int first_diagonal = start_square + dir*7;
    int second_diagonal = start_square + dir*9;
    if (IsEnemyPiece(squares[first_diagonal]) &&
    abs(start_square%8 - first_diagonal%8) == 1) {
        if (is_next_rank_promo) {
            move_set.push_back(CreateMove(start_square, first_diagonal, CAPTURE_MOVE | PROMO_QUEEN));
            move_set.push_back(CreateMove(start_square, first_diagonal, CAPTURE_MOVE | PROMO_BISHOP));
            move_set.push_back(CreateMove(start_square, first_diagonal, CAPTURE_MOVE | PROMO_KNIGHT));
            move_set.push_back(CreateMove(start_square, first_diagonal, CAPTURE_MOVE | PROMO_ROOK));
        } else {
            move_set.push_back(CreateMove(start_square, first_diagonal, CAPTURE_MOVE));
        }
    }
    if (IsEnemyPiece(squares[second_diagonal]) &&
    abs(start_square%8 - second_diagonal%8) == 1) {
        if (is_next_rank_promo) {
            move_set.push_back(CreateMove(start_square, second_diagonal, CAPTURE_MOVE | PROMO_QUEEN));
            move_set.push_back(CreateMove(start_square, second_diagonal, CAPTURE_MOVE | PROMO_BISHOP));
            move_set.push_back(CreateMove(start_square, second_diagonal, CAPTURE_MOVE | PROMO_KNIGHT));
            move_set.push_back(CreateMove(start_square, second_diagonal, CAPTURE_MOVE | PROMO_ROOK));
        } else {
            move_set.push_back(CreateMove(start_square, second_diagonal, CAPTURE_MOVE));
        }
    }

    // en passant
    if (half_moves == EnPassant.IsAvailable) {
        if (first_diagonal == EnPassant.place &&
        abs(start_square%8 - first_diagonal%8) == 1) {
            move_set.push_back(CreateMove(start_square, first_diagonal, EN_PASSANT));
        } else if(second_diagonal == EnPassant.place &&
        abs(start_square%8 - second_diagonal%8) == 1) {
            move_set.push_back(CreateMove(start_square, second_diagonal, EN_PASSANT));
        }
    }
}

void Board::HandlePawnMove (int p, Move move) {
    int dir = (IsWhite(p))? 1 : -1;
    uint8_t start_pos, final_pos;

    start_pos = GetStartPos(move);
    final_pos = GetFinalPos(move);

    if (abs(final_pos/8 - start_pos/8) == 2) {             // create en passant opportunity 
        EnPassant.IsAvailable = half_moves + 1;
        EnPassant.place = start_pos + dir*8;
    } else if (IsEnPassant(move)) {                        // En Passant
        squares[final_pos + dir*-1 * 8] = EMPTY;
    } else if (IsPromotion(move)) {                        // Promotion
        squares[final_pos] = GetPromotionPieceFromMove(move) | GetColor(p);
    }
}

void Board::GenerateKingMoves (int start_square) {
    int start_file = start_square % 8;

    for (int dir = 0; dir < 8; ++dir) {
        int next_square = start_square + directions[dir];
        int next_file = next_square % 8;

        if (next_square >= 0 && next_square < 64) {
            if (abs(start_file - next_file) < 2) {
                if (squares[next_square] == EMPTY) {
                    move_set.push_back(CreateMove(start_square, next_square, QUIET_MOVE));
                } else if (IsAllyPiece(squares[next_square])) {
                    continue;
                } else if (IsEnemyPiece(squares[next_square])) {
                    move_set.push_back(CreateMove(start_square, next_square, CAPTURE_MOVE));
                }
            }   
        }
    }
}

void Board::GenerateHorseMoves (int start_square) {
    int start_file = start_square % 8;

    for (int move : knight_moves) {
        int next_square = start_square + move;
        int next_file = next_square % 8;

        if (next_square >= 0 && next_square < 64) { // Make sure its on the board
            if (abs(start_file - next_file) < 3) {  // Make sure no warping
                if (squares[next_square] == EMPTY) {
                    move_set.push_back(CreateMove(start_square, next_square, QUIET_MOVE));
                } else if (IsAllyPiece(squares[next_square])) {
                    continue;
                } else if (IsEnemyPiece(squares[next_square])) {
                    move_set.push_back(CreateMove(start_square, next_square, CAPTURE_MOVE));
                }
            }
        }
    }
}

void Board::GenerateSlidingMoves (int p, int start_square) {
    int diri = 0;
    int diri_max = 8;

    int raw_p = RawPiece(p);

    if (raw_p == ROOK) diri_max = 4;
    else if (raw_p == BISHOP) diri = 4;
    
    for (; diri < diri_max; ++diri) {
        for (int i = 0; i < distance_to_edge[start_square][diri]; ++i) {
            int next_square = start_square + (directions[diri] * (i + 1));
            
            if (squares[next_square] == EMPTY) {
                move_set.push_back(CreateMove(start_square, next_square, QUIET_MOVE));
            } else if (IsAllyPiece(squares[next_square])) {
                break;
            } else if (IsEnemyPiece(squares[next_square])) {
                move_set.push_back(CreateMove(start_square, next_square, CAPTURE_MOVE));
                break;
            }
        }
    }
}

void Board::GenerateMovesForSquare (int start_square) {
    Piece _piece = squares[start_square];
    Piece _raw   = RawPiece(_piece);

    if (IsAllyPiece(_piece)) {
        if (IsSlidingPiece(_piece)) {
            GenerateSlidingMoves(_piece, start_square);
        } else if (_raw == KNIGHT) {
            GenerateHorseMoves(start_square);
        } else if (_raw == KING) {
            GenerateKingMoves(start_square);
        } else if (_raw == PAWN) {
            GeneratePawnMoves(_piece, start_square);
        }
    }
}

void Board::GenerateAllMoves () {
    move_set.clear();

    for (int i = 0; i < 64; ++i) {
        GenerateMovesForSquare(i);
    }
}

void Board::ExecuteMove (Move move) {
    uint8_t start_pos, final_pos;

    start_pos = GetStartPos(move);
    final_pos = GetFinalPos(move);

    tracker->NewMove(move,squares[final_pos],NON);

    squares[final_pos] = squares[start_pos];
    squares[start_pos] = EMPTY;

    if (RawPiece(squares[final_pos]) == PAWN)
        HandlePawnMove(squares[final_pos], move);
    
    EndTurn();
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
