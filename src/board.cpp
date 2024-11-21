#include "board.hpp"
#include "piece.hpp"
#include "move.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

// TODO:
// 50 move rule
// rework EnPassant struct into just "place" variable (-1 means no en passant)

struct {
    int IsAvailable = -1;
    int place = -1;
} EnPassant;

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
    int stage = 0;

    while (char c = *FEN++) {
        if (c == ' ') {
            stage++;
            continue;
        }

        switch (stage) {
            case 0: {
                if (c == '/') {
                    index -= 16;      // jump back to previous rank
                } else if (isdigit(c)) {
                    index += c - '0'; // from char to int (jump number of files)
                } else {
                    squares[index++] = CharToPiece(c);
                }
            } break;
            case 1: {
                color_to_play = (c == 'w')? WHITE : BLACK;
            } break;
            case 2: {
                switch (c) {
                    case 'K':
                        castling_rights |= 0b1000;
                        break;
                    case 'Q':
                        castling_rights |= 0b0100;
                        break;
                    case 'k':
                        castling_rights |= 0b0010;
                        break;
                    case 'q':
                        castling_rights |= 0b0001;
                        break;
                    default:
                        printf("Error while loading castling rights\n");
                }
            } break;
            case 3: {
                if (c == '-') continue;
                EnPassant.IsAvailable = 1;
                EnPassant.place = NotationToBoardIndex(HelperClass::ToDigit(*FEN), c);
            } break;
            case 4: {
                printf("FEN: %c\n", *FEN);
                fifty_move_rule = HelperClass::GetNumberFromPointer(FEN - 1);
            } break;
            case 5: {
                half_moves = FullMovesToHalfMoves(HelperClass::GetNumberFromPointer(FEN - 1), IsWhiteToPlay());
                if (EnPassant.IsAvailable != -1) EnPassant.IsAvailable = half_moves;
            }
            case 6: {
                printf("Loaded game:\nColor to play: %c\nCastling rights: %d\nEn passant: %d\n50 move clock: %d\nFull move counter: %d\n",
                IsWhiteToPlay()? 'w' : 'b',
                castling_rights,
                EnPassant.place,
                fifty_move_rule,
                HalfMovesToFullMoves(half_moves));
            } break;
        }
    }
}


void Board::RestartBoard() {
    tracker.ResetTracker();
    LoadBoard(CUSTOM_POS);
    GenerateAllMoves();
}

void Board::EndTurn () {
    // Stop clock (not necessarily)

    // Check if its stalemate or checkmate or draw by other means
    if (fifty_move_rule == 50) {
        // Game is a draw.
        printf("Game is draw by 50 move rule.\n");
    }
    
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
            // IMPORTANT: This specific order of move generation is very important
            // and should not be altered in any way shape or form.
            // This may not be the best way to create a structure that user input
            // relies on but is what we got for now.
            move_set.push_back(CreateMove(start_square, forward_square, PROMO_QUEEN));
            move_set.push_back(CreateMove(start_square, forward_square, PROMO_BISHOP));
            move_set.push_back(CreateMove(start_square, forward_square, PROMO_KNIGHT));
            move_set.push_back(CreateMove(start_square, forward_square, PROMO_ROOK));
        } else {
            move_set.push_back(CreateMove(start_square, forward_square, QUIET_MOVE));
            if (is_first_move && squares[start_square + dir*16] == EMPTY)
                move_set.push_back(CreateMove(start_square, start_square + dir*16, DOUBLE_PAWN));
        }
    }

    int first_diagonal = start_square + dir*7;
    int second_diagonal = start_square + dir*9;
    if (IsEnemyPiece(squares[first_diagonal]) &&
    abs(start_square%8 - first_diagonal%8) == 1) {
        if (is_next_rank_promo) {
            move_set.push_back(CreateMove(start_square, first_diagonal, CAPTURE_MOVE | PROMO_QUEEN, PR_CAS, squares[first_diagonal]));
            move_set.push_back(CreateMove(start_square, first_diagonal, CAPTURE_MOVE | PROMO_BISHOP, PR_CAS, squares[first_diagonal]));
            move_set.push_back(CreateMove(start_square, first_diagonal, CAPTURE_MOVE | PROMO_KNIGHT, PR_CAS, squares[first_diagonal]));
            move_set.push_back(CreateMove(start_square, first_diagonal, CAPTURE_MOVE | PROMO_ROOK, PR_CAS, squares[first_diagonal]));
        } else {
            move_set.push_back(CreateMove(start_square, first_diagonal, CAPTURE_MOVE, PR_CAS, squares[first_diagonal]));
        }
    }
    if (IsEnemyPiece(squares[second_diagonal]) &&
    abs(start_square%8 - second_diagonal%8) == 1) {
        if (is_next_rank_promo) {
            move_set.push_back(CreateMove(start_square, second_diagonal, CAPTURE_MOVE | PROMO_QUEEN, PR_CAS, squares[second_diagonal]));
            move_set.push_back(CreateMove(start_square, second_diagonal, CAPTURE_MOVE | PROMO_BISHOP, PR_CAS, squares[second_diagonal]));
            move_set.push_back(CreateMove(start_square, second_diagonal, CAPTURE_MOVE | PROMO_KNIGHT, PR_CAS, squares[second_diagonal]));
            move_set.push_back(CreateMove(start_square, second_diagonal, CAPTURE_MOVE | PROMO_ROOK, PR_CAS, squares[second_diagonal]));
        } else {
            move_set.push_back(CreateMove(start_square, second_diagonal, CAPTURE_MOVE, PR_CAS, squares[second_diagonal]));
        }
    }

    // en passant
    if (half_moves == EnPassant.IsAvailable) {
        if (first_diagonal == EnPassant.place &&
        abs(start_square%8 - first_diagonal%8) == 1) {
            move_set.push_back(CreateMove(start_square, first_diagonal, EN_PASSANT, PR_CAS, PAWN));
        } else if(second_diagonal == EnPassant.place &&
        abs(start_square%8 - second_diagonal%8) == 1) {
            move_set.push_back(CreateMove(start_square, second_diagonal, EN_PASSANT, PR_CAS, PAWN));
        }
    }
}

void Board::HandlePawnMove (Piece color, Move move) {
    int dir = (color == WHITE)? 1 : -1;
    uint8_t start_pos, final_pos;

    start_pos = GetStartPos(move);
    final_pos = GetFinalPos(move);

    if (IsDoublePawn(move)) {                     // create en passant opportunity 
        EnPassant.IsAvailable = half_moves + 1;
        EnPassant.place = start_pos + dir*8;
    } else if (IsEnPassant(move)) {               // En Passant
        squares[final_pos + dir*-1 * 8] = EMPTY;
    } else if (IsPromotion(move)) {               // Promotion
        squares[final_pos] = GetPromotionPieceFromMove(move) | color;
    }
}

void Board::GenerateKingMoves (int start_square) {
    int start_file = start_square % 8;
    uint8_t change_castle_rights = (IsWhiteToPlay())? 0b0011 : 0b1100;

    for (int dir = 0; dir < 8; ++dir) {
        int next_square = start_square + directions[dir];
        int next_file = next_square % 8;

        if (next_square >= 0 && next_square < 64) {
            if (abs(start_file - next_file) < 2) {
                if (squares[next_square] == EMPTY) {
                    move_set.push_back(CreateMove(start_square, next_square, QUIET_MOVE, change_castle_rights));
                } else if (IsAllyPiece(squares[next_square])) {
                    continue;
                } else if (IsEnemyPiece(squares[next_square])) {
                    move_set.push_back(CreateMove(start_square, next_square, CAPTURE_MOVE, change_castle_rights, squares[next_square]));
                }
            }   
        }
    }

    bool K = true, Q = true;
    if (IsWhiteToPlay()) {
        if (castling_rights & 0b1000) {
            for (uint8_t s : white_kingside) {
                if (squares[s] == EMPTY) continue;
                K = false;
                break;
            }
            if (K) move_set.push_back(CreateMove(start_square, WKSC_SQUARE, CASTLE_KS, change_castle_rights));
        }
        if (castling_rights & 0b0100) {
            for (uint8_t s : white_queenside) {
                if (squares[s] == EMPTY) continue;
                Q = false;
                break;
            }
            if (Q) move_set.push_back(CreateMove(start_square, WQSC_SQUARE, CASTLE_QS, change_castle_rights));
        }
    } else {
        if (castling_rights & 0b0010) {
            for (uint8_t s : black_kingside) {
                if (squares[s] == EMPTY) continue;
                K = false;
                break;
            }
            if (K) move_set.push_back(CreateMove(start_square, BKSC_SQUARE, CASTLE_KS, change_castle_rights));
        }
        if (castling_rights & 0b0001) {
            for (uint8_t s : black_queenside) {
                if (squares[s] == EMPTY) continue;
                Q = false;
                break;
            }
            if (Q) move_set.push_back(CreateMove(start_square, BQSC_SQUARE, CASTLE_QS, change_castle_rights));
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
                    move_set.push_back(CreateMove(start_square, next_square, CAPTURE_MOVE, PR_CAS, squares[next_square]));
                }
            }
        }
    }
}

void Board::GenerateSlidingMoves (int p, int start_square) {
    int diri = 0;
    int diri_max = 8;
    uint8_t change_castling_rights = PR_CAS;

    int raw_p = RawPiece(p);

    if (raw_p == ROOK) diri_max = 4;
    else if (raw_p == BISHOP) diri = 4;
    
    if (raw_p == ROOK && castling_rights) {
        if (start_square == 7) {
            change_castling_rights = 0b0111;
        } else if (start_square == 0) {
            change_castling_rights = 0b1011;
        } else if (start_square == 63) {
            change_castling_rights = 0b1101;
        } else if (start_square == 56) {
            change_castling_rights = 0b1110;
        }
    }

    for (; diri < diri_max; ++diri) {
        for (int i = 0; i < distance_to_edge[start_square][diri]; ++i) {
            int next_square = start_square + (directions[diri] * (i + 1));
            
            if (squares[next_square] == EMPTY) {
                move_set.push_back(CreateMove(start_square, next_square, QUIET_MOVE, change_castling_rights));
            } else if (IsAllyPiece(squares[next_square])) {
                break;
            } else if (IsEnemyPiece(squares[next_square])) {
                move_set.push_back(CreateMove(start_square, next_square, CAPTURE_MOVE, change_castling_rights, squares[next_square]));
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
    /* Make the move */
    uint8_t start_pos, final_pos, moved_piece;
    bool white_color;

    start_pos = GetStartPos(move);
    final_pos = GetFinalPos(move);
    moved_piece = squares[start_pos];
    white_color = IsWhiteToPlay();

    tracker.PushMove(move);

    squares[final_pos] = moved_piece;
    squares[start_pos] = EMPTY;
    /* Make the move */

    /* Update board state */
    castling_rights &= GetCastleRights(move);

    if (RawPiece(moved_piece) == PAWN) {
        HandlePawnMove(color_to_play, move);
    } else if (RawPiece(moved_piece) == KING) {
        if (white_color) white_king_pos = final_pos;
        else             black_king_pos = final_pos;

        int KSC_SQUARE = (white_color)? WKSC_SQUARE : BKSC_SQUARE;
        int QSC_SQUARE = (white_color)? WQSC_SQUARE : BQSC_SQUARE;
        if (IsKSCastle(move)) {
            squares[KSC_SQUARE - 1] = squares[KSC_SQUARE + 1];
            squares[KSC_SQUARE + 1] = EMPTY;
        } else if (IsQSCastle(move)) {
            squares[QSC_SQUARE + 1] = squares[QSC_SQUARE - 2];
            squares[QSC_SQUARE - 2] = EMPTY;
        }
    }

    if (IsCapture(move) || RawPiece(moved_piece) == PAWN) {
        fifty_move_rule = 0;
    } else {
        fifty_move_rule++;
    }

    half_moves++;
    color_to_play = SwitchColor(color_to_play);
    /* Update board state */
    
    EndTurn();
}

void Board::UndoMove (Move move) {
    uint8_t start_pos, final_pos, moved_piece, moved_piece_color, captured_piece_color, captured_piece = 0;

    start_pos = GetStartPos(move);
    final_pos = GetFinalPos(move);
    moved_piece = squares[final_pos];
    moved_piece_color = SwitchColor(color_to_play);
    captured_piece_color = color_to_play;
    if (IsCapture(move)) captured_piece = GetCapturedPieceFromMove(move) | captured_piece_color;
    int dir = (captured_piece_color == WHITE)? 1 : -1;

    tracker.UndoMove();

    squares[start_pos] = moved_piece;

    if (IsEnPassant(move)) {
        squares[final_pos] = EMPTY;
        squares[final_pos + 16 * dir] = PAWN | captured_piece_color;
    } else {
        squares[final_pos] = captured_piece;
    }

    /* Update Board State */
    castling_rights |= ~GetCastleRights(move);

    if (RawPiece(moved_piece) == PAWN) {
        HandlePawnMove(color_to_play, move);
    } else if (RawPiece(moved_piece) == KING) {
        if (IsWhite(moved_piece)) white_king_pos = final_pos;
        else                      black_king_pos = final_pos;

        int KSC_SQUARE = IsWhite(moved_piece)? WKSC_SQUARE : BKSC_SQUARE;
        int QSC_SQUARE = IsWhite(moved_piece)? WQSC_SQUARE : BQSC_SQUARE;
        if (IsKSCastle(move)) {
            squares[KSC_SQUARE + 1] = squares[KSC_SQUARE - 1];
            squares[KSC_SQUARE - 1] = EMPTY;
        } else if (IsQSCastle(move)) {
            squares[QSC_SQUARE - 2] = squares[QSC_SQUARE + 1];
            squares[QSC_SQUARE + 1] = EMPTY;
        }
    }

    if (IsDoublePawn(move)) {                     // remove en passant opportunity
        EnPassant.IsAvailable = -1;
    } else if (IsEnPassant(move)) {               // create en passant opportunity
        EnPassant.IsAvailable = half_moves;
        EnPassant.place = final_pos;
    } else if (IsPromotion(move)) {               // promotion
        squares[start_pos] = PAWN | moved_piece_color;
    }

    half_moves--;
    color_to_play = SwitchColor(color_to_play);
    /* Update Board State */
    
    EndTurn();
}

int Board::NotationToBoardIndex (int rank, char file) {
    return (rank - 1) * 8 + (file - 'a');
}

int Board::NotationToBoardIndex (int rank, int file) {
    return rank * 8 + file;
}

int Board::HalfMovesToFullMoves (int half_moves) {
    return (half_moves + 2) / 2;
}

int Board::FullMovesToHalfMoves (int full_moves, bool isWhiteToPlay) {
    return (full_moves * 2) - ((isWhiteToPlay)? 2 : 1);
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

// ====================   Game Tracker   =========================== //

// GameTracker::GameTracker(){
//     curr_pos = Moves.end();
//     init_state_summary = 0b00000000;
// }

/* i dont think we'll need a reference to the board here
GameTracker::GameTracker(Board *board): GameTracker(){
    this->board = board;
    this->SetFEN();
}

GameTracker::GameTracker(Board *board , std::string FEN): GameTracker(){
    this->board = board;
    this->SetFEN(FEN);
}

void GameTracker::SetFEN(std::string FEN){
    ResetTracker();
    start_pos = FEN;
    this->board->LoadBoard(FEN.c_str());
}
*/




size_t GameTracker::MovesCount() {
    return (move_history.size());
}

bool GameTracker::IsEmpty(){
    return (!move_history.size());
}

Move GameTracker::GetCurrMove() {
    return move_history.back();
}

void GameTracker::ImportPGN(std::string PGN){

}

std::string GameTracker::ExportPGN(){
    return "NOT FINISHED";
}

void GameTracker::PrintTracker () {
    std::cout << "==================" << std::endl;
    for (Move m : move_history) {
        printf("%x\n", m);
    }
}


/* At least for now we won't need to track WHERE we are on the list
void GameTracker::Prev () {

}

void GameTracker::Next () {

}

void GameTracker::GoToMove (int MoveNumber) {

}

bool GameTracker::IsLastMove () {
    return curr_pos == Moves.rbegin().base();
}

bool GameTracker::IsFirstMove () {
    return curr_pos == Moves.begin();
}
*/

void GameTracker::PushMove(Move mv) {
    move_history.push_back(mv);
}

void GameTracker::UndoMove() {
    move_history.pop_back();
}

void GameTracker::ResetTracker() {
    move_history.clear();
    //curr_pos = Moves.end();
}
