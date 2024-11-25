#include "board.hpp"
#include "piece.hpp"
#include "move.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

// TODO:
// 50 move rule
// 3 fold repitition

// Board

// TODO: convert current position to FEN string
void Board::PrintBoard () {
    // DOESN'T WORK AS INTENDED ANYMORE
    printf("\nColor to play: %c\nCastling rights: %d\nEn passant: %d\n50 move clock: %d\nFull move counter: %d\n",
        IsWhiteToPlay()? 'w' : 'b',
        castling_rights,
        enpassant_place,
        fifty_move_rule,
        HalfMovesToFullMoves(half_moves));
}

void Board::LoadBoard (const char* FEN) {
    for (Piece &p : squares) {
        p = EMPTY;
    }

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
                    Piece _p = CharToPiece(c);
                    // setting king positions from fen string
                    if      (_p == W_KING) white_king_pos = index;
                    else if (_p == B_KING) black_king_pos = index;
                    squares[index++] = _p;
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
                enpassant_place = NotationToBoardIndex(HelperClass::ToDigit(*FEN), c);
            } break;
            case 4: {
                fifty_move_rule = HelperClass::GetNumberFromPointer(FEN - 1);
            } break;
            case 5: {
                half_moves = FullMovesToHalfMoves(HelperClass::GetNumberFromPointer(FEN - 1), IsWhiteToPlay());
            } break;
        }
    }
}


void Board::RestartBoard (const char* position) {
    tracker.ResetTracker();
    LoadBoard(position);
}

// Not needed right now
// void Board::EndTurn () {
//     // Stop clock (not necessarily)

//     // Check if its stalemate or checkmate or draw by other means
//     if (fifty_move_rule == 50) {
//         // Game is a draw.
//         printf("Game is draw by 50 move rule.\n");
//     }
// }

void Board::GetAllMoves (MoveArray* moves) {
    moves->Clear();
    uint8_t curr_castle_rights = castling_rights;

    for (int start_square = 0; start_square < 64; ++start_square) {
        Piece _piece = squares[start_square];

        if (!IsAllyPiece(_piece)) continue; // Function guard

        // Initialize needed variables
        Piece _raw = RawPiece(_piece);
        int start_file = start_square % 8;

        // Generate moves by piece type
        if (IsSlidingPiece(_piece)) {
            int diri = 0;
            int diri_max = 8;

            if (_raw == ROOK) diri_max = 4;
            else if (_raw == BISHOP) diri = 4;

            for (; diri < diri_max; ++diri) {
                for (int i = 0; i < distance_to_edge[start_square][diri]; ++i) {
                    int next_square = start_square + (directions[diri] * (i + 1));
                    
                    if (squares[next_square] == EMPTY) {
                        moves->AddMove(CreateMove(start_square, next_square, QUIET_MOVE, curr_castle_rights));
                    } else if (IsAllyPiece(squares[next_square])) {
                        break;
                    } else if (IsEnemyPiece(squares[next_square])) {
                        moves->AddMove(CreateMove(start_square, next_square, CAPTURE_MOVE, curr_castle_rights, squares[next_square]));
                        break;
                    }
                }
            }
        } else if (_raw == KNIGHT) {
            for (int move : knight_moves) {
                int next_square = start_square + move;
                int next_file = next_square % 8;

                if (next_square >= 0 && next_square < 64) { // Make sure its on the board
                    if (abs(start_file - next_file) < 3) {  // Make sure no warping
                        if (squares[next_square] == EMPTY) {
                            moves->AddMove(CreateMove(start_square, next_square, QUIET_MOVE, curr_castle_rights));
                        } else if (IsAllyPiece(squares[next_square])) {
                            continue;
                        } else if (IsEnemyPiece(squares[next_square])) {
                            moves->AddMove(CreateMove(start_square, next_square, CAPTURE_MOVE, curr_castle_rights, squares[next_square]));
                        }
                    }
                }
            }
        } else if (_raw == KING) {
            for (int dir = 0; dir < 8; ++dir) {
                int next_square = start_square + directions[dir];
                int next_file = next_square % 8;

                if (next_square >= 0 && next_square < 64) {
                    if (abs(start_file - next_file) < 2) {
                        if (squares[next_square] == EMPTY) {
                            moves->AddMove(CreateMove(start_square, next_square, QUIET_MOVE, curr_castle_rights));
                        } else if (IsAllyPiece(squares[next_square])) {
                            continue;
                        } else if (IsEnemyPiece(squares[next_square])) {
                            moves->AddMove(CreateMove(start_square, next_square, CAPTURE_MOVE, curr_castle_rights, squares[next_square]));
                        }
                    }   
                }
            }

            bool K = true, Q = true;
            if (!IsInCheck()) {
            if (IsWhiteToPlay()) {
                if (castling_rights & 0b1000) {
                    for (uint8_t s : white_kingside) {
                        if (squares[s] == EMPTY) continue;
                        K = false;
                        break;
                    }
                    if (IsSquareAttacked(WKSC_SQUARE - 1)) K = false;
                    if (K) moves->AddMove(CreateMove(start_square, WKSC_SQUARE, CASTLE_KS, curr_castle_rights));
                }
                if (castling_rights & 0b0100) {
                    for (uint8_t s : white_queenside) {
                        if (squares[s] == EMPTY) continue;
                        Q = false;
                        break;
                    }
                    if (IsSquareAttacked(WQSC_SQUARE + 1)) Q = false;
                    if (Q) moves->AddMove(CreateMove(start_square, WQSC_SQUARE, CASTLE_QS, curr_castle_rights));
                }
            } else {
                if (castling_rights & 0b0010) {
                    for (uint8_t s : black_kingside) {
                        if (squares[s] == EMPTY) continue;
                        K = false;
                        break;
                    }
                    if (IsSquareAttacked(BKSC_SQUARE - 1)) K = false;
                    if (K) moves->AddMove(CreateMove(start_square, BKSC_SQUARE, CASTLE_KS, curr_castle_rights));
                }
                if (castling_rights & 0b0001) {
                    for (uint8_t s : black_queenside) {
                        if (squares[s] == EMPTY) continue;
                        Q = false;
                        break;
                    }
                    if (IsSquareAttacked(BQSC_SQUARE + 1)) Q = false;
                    if (Q) moves->AddMove(CreateMove(start_square, BQSC_SQUARE, CASTLE_QS, curr_castle_rights));
                }
            }
            }
        } else if (_raw == PAWN) {
            short dir = (IsWhiteToPlay())? 1 : -1;
            short rank = start_square / 8;
            int forward_square = start_square + dir*8;

            bool is_first_move = (IsWhiteToPlay())? rank == 1 : rank == 6;
            bool is_next_rank_promo = (forward_square / 8 == 0 || forward_square / 8 == 7);

            if(squares[forward_square] == EMPTY) {
                if (is_next_rank_promo) {
                    // IMPORTANT: This specific order of move generation is very important
                    // and should not be altered in any way shape or form.
                    // This may not be the best way to create a structure that user input
                    // relies on but is what we got for now.
                    moves->AddMove(CreateMove(start_square, forward_square, PROMO_QUEEN, curr_castle_rights));
                    moves->AddMove(CreateMove(start_square, forward_square, PROMO_BISHOP, curr_castle_rights));
                    moves->AddMove(CreateMove(start_square, forward_square, PROMO_KNIGHT, curr_castle_rights));
                    moves->AddMove(CreateMove(start_square, forward_square, PROMO_ROOK, curr_castle_rights));
                } else {
                    moves->AddMove(CreateMove(start_square, forward_square, QUIET_MOVE, curr_castle_rights));
                    if (is_first_move && squares[start_square + dir*16] == EMPTY)
                        moves->AddMove(CreateMove(start_square, start_square + dir*16, DOUBLE_PAWN, curr_castle_rights));
                }
            }

            int first_diagonal = start_square + dir*7;
            int second_diagonal = start_square + dir*9;
            if (IsEnemyPiece(squares[first_diagonal]) &&
            abs(start_file - first_diagonal%8) == 1) {
                if (is_next_rank_promo) {
                    moves->AddMove(CreateMove(start_square, first_diagonal, CAPTURE_MOVE | PROMO_QUEEN, curr_castle_rights, squares[first_diagonal]));
                    moves->AddMove(CreateMove(start_square, first_diagonal, CAPTURE_MOVE | PROMO_BISHOP, curr_castle_rights, squares[first_diagonal]));
                    moves->AddMove(CreateMove(start_square, first_diagonal, CAPTURE_MOVE | PROMO_KNIGHT, curr_castle_rights, squares[first_diagonal]));
                    moves->AddMove(CreateMove(start_square, first_diagonal, CAPTURE_MOVE | PROMO_ROOK, curr_castle_rights, squares[first_diagonal]));
                } else {
                    moves->AddMove(CreateMove(start_square, first_diagonal, CAPTURE_MOVE, curr_castle_rights, squares[first_diagonal]));
                }
            }
            if (IsEnemyPiece(squares[second_diagonal]) &&
            abs(start_file - second_diagonal%8) == 1) {
                if (is_next_rank_promo) {
                    moves->AddMove(CreateMove(start_square, second_diagonal, CAPTURE_MOVE | PROMO_QUEEN, curr_castle_rights, squares[second_diagonal]));
                    moves->AddMove(CreateMove(start_square, second_diagonal, CAPTURE_MOVE | PROMO_BISHOP, curr_castle_rights, squares[second_diagonal]));
                    moves->AddMove(CreateMove(start_square, second_diagonal, CAPTURE_MOVE | PROMO_KNIGHT, curr_castle_rights, squares[second_diagonal]));
                    moves->AddMove(CreateMove(start_square, second_diagonal, CAPTURE_MOVE | PROMO_ROOK, curr_castle_rights, squares[second_diagonal]));
                } else {
                    moves->AddMove(CreateMove(start_square, second_diagonal, CAPTURE_MOVE, curr_castle_rights, squares[second_diagonal]));
                }
            }

            // en passant
            if (EnPassantExists()) {
                if (first_diagonal == enpassant_place &&
                abs(start_file - first_diagonal%8) == 1) {
                    moves->AddMove(CreateMove(start_square, first_diagonal, EN_PASSANT, curr_castle_rights, PAWN));
                } else if(second_diagonal == enpassant_place &&
                abs(start_file - second_diagonal%8) == 1) {
                    moves->AddMove(CreateMove(start_square, second_diagonal, EN_PASSANT, curr_castle_rights, PAWN));
                }
            }
        }
    }
}

// TODO: Optimize by applying pins and
// keeping track of attacked squares
void Board::GetLegalMoves (MoveArray* moves) {
    GetAllMoves(moves);

    for (size_t i = 0; i < moves->Size(); ++i) {
        ExecuteMove(moves->At(i));
        color_to_play = SwitchColor(color_to_play);
        if (IsInCheck()) {
            moves->NullifyMove(i);
        }
        color_to_play = SwitchColor(color_to_play);
        UndoMove(tracker.GetCurrMove());
    }
}

bool Board::IsInCheck () {
    return IsWhiteToPlay()? IsSquareAttacked(white_king_pos) : IsSquareAttacked(black_king_pos);
}

bool Board::IsSquareAttacked (int square_index) {
    Piece opp_color = SwitchColor(color_to_play);

    for (int m : knight_moves) {
        int next_square = square_index + m;
        if (next_square > 63 || next_square < 0) continue;
        if (abs(square_index%8 - next_square%8) > 2) continue;
        if (squares[next_square] == (KNIGHT | opp_color)) return true;
    }

    for (int diri = 0; diri < 8; ++diri) {
        for (int i = 0; i < distance_to_edge[square_index][diri]; ++i) {
            Piece next_square = squares[square_index + (directions[diri] * (i + 1))];
            if (IsOfColor(next_square, color_to_play)) break;
            else if (IsOfColor(next_square, opp_color)) {
                Piece ptype = RawPiece(next_square);

                if ( (ptype == QUEEN) ||
                     (ptype == ROOK && diri < 4) ||
                     (ptype == BISHOP && diri >= 4) ||
                     (ptype == KING && i == 0) ||
                     (next_square == B_PAWN && i == 0 && (directions[diri] == 7 || directions[diri] == 9)) ||
                     (next_square == W_PAWN && i == 0 && (directions[diri] == -7 || directions[diri] == -9))
                ) {
                    return true;
                } else {
                    break;
                }
            }
        }
    }

    return false;
}

void Board::ExecuteMove (Move move) {
    if (IsNullMove(move)) return;

    /* Make the move */
    uint8_t start_pos, final_pos, moved_piece, raw_moved_piece, captured_piece;
    bool white_color;

    start_pos = GetStartPos(move);
    final_pos = GetFinalPos(move);
    moved_piece = squares[start_pos];
    raw_moved_piece = RawPiece(moved_piece);
    captured_piece = GetCapturedPieceFromMove(move);
    white_color = IsWhiteToPlay();

    tracker.PushMove(move);

    squares[final_pos] = moved_piece;
    squares[start_pos] = EMPTY;
    /* Make the move */

    /* Update board state */
    if (RawPiece(captured_piece) == ROOK) {
        if (final_pos == 7) {
            castling_rights &= 0b0111;
        } else if (final_pos == 0) {
            castling_rights &= 0b1011;
        } else if (final_pos == 63) {
            castling_rights &= 0b1101;
        } else if (final_pos == 56) {
            castling_rights &= 0b1110;
        }
    }

    if (raw_moved_piece == ROOK) {
        if (start_pos == 7) {
            castling_rights &= 0b0111;
        } else if (start_pos == 0) {
            castling_rights &= 0b1011;
        } else if (start_pos == 63) {
            castling_rights &= 0b1101;
        } else if (start_pos == 56) {
            castling_rights &= 0b1110;
        }
    } else if (raw_moved_piece == KING) {
        uint8_t change_castle_rights = white_color? 0b0011 : 0b1100;
        castling_rights &= change_castle_rights;
    }

    if (raw_moved_piece == PAWN) {
        int dir = (white_color)? 1 : -1;
        uint8_t start_pos, final_pos;

        start_pos = GetStartPos(move);
        final_pos = GetFinalPos(move);

        if (IsDoublePawn(move)) {                     // create en passant opportunity 
            enpassant_place = start_pos + dir*8;
        } else if (IsEnPassant(move)) {               // En Passant
            squares[final_pos + dir*-1 * 8] = EMPTY;
        } else if (IsPromotion(move)) {               // Promotion
            squares[final_pos] = GetPromotionPieceFromMove(move) | color_to_play;
        }
    } else if (raw_moved_piece == KING) {
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

    if (!IsDoublePawn(move)) enpassant_place = -1;

    if (IsCapture(move) || raw_moved_piece == PAWN) {
        fifty_move_rule = 0;
    } else {
        fifty_move_rule++;
    }

    half_moves++;
    color_to_play = SwitchColor(color_to_play);
    /* Update board state */
}

void Board::UndoMove (Move move) {
    if (IsNullMove(move)) return;

    uint8_t start_pos, final_pos, moved_piece, moved_piece_color, captured_piece_color, captured_piece = 0;

    start_pos = GetStartPos(move);
    final_pos = GetFinalPos(move);
    moved_piece = squares[final_pos];
    moved_piece_color = SwitchColor(color_to_play);
    captured_piece_color = color_to_play;
    if (IsCapture(move)) captured_piece = GetCapturedPieceFromMove(move) | captured_piece_color;
    int dir = (captured_piece_color == WHITE)? 1 : -1;

    tracker.UndoMove();

    // replace the piece to its original square
    // if the move was a promotion, demote the piece
    if (IsPromotion(move)) squares[start_pos] = PAWN | moved_piece_color;
    else                   squares[start_pos] = moved_piece;

    // change the final square depending on move type
    if (IsEnPassant(move)) {
        squares[final_pos] = EMPTY;
        squares[final_pos + 8 * dir] = PAWN | captured_piece_color;
    } else {
        squares[final_pos] = captured_piece;
    }
    
    // check if the move before the undone move was a double pawn push
    // if so, then set the en passant square
    Move past_move = tracker.GetCurrMove();
    if (!tracker.IsEmpty() && IsDoublePawn(past_move)) {
        enpassant_place = GetStartPos(past_move) + (IsWhiteToPlay()? 8 : -8);
    } else {
        enpassant_place = -1;
    }

    /* Update Board State */
    castling_rights |= GetCastleRights(move);

    if (RawPiece(moved_piece) == KING) {
        if (IsWhite(moved_piece)) white_king_pos = start_pos;
        else                      black_king_pos = start_pos;

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

    half_moves--;
    color_to_play = SwitchColor(color_to_play);
    /* Update Board State */
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

// Function that was used to calculate distances_to_edge[][] array.
//
// void Board::PreCalculateDistancesToEdgeOfBoard () {
//     for (int rank = 0; rank < 8; ++rank) {
//         for (int file = 0; file < 8; ++file) {
//             int north = rank;
//             int south = 7 - rank;
//             int east = 7 - file;
//             int west = file;

//             int* dte = distance_to_edge[NotationToBoardIndex(rank, file)];

//             dte[0] = south;
//             dte[1] = north;
//             dte[2] = east;
//             dte[3] = west;
//             dte[4] = std::min(south, west);
//             dte[5] = std::min(north, east);
//             dte[6] = std::min(south, east);
//             dte[7] = std::min(north, west);
//         }
//     }
// }

// ====================   Game Tracker   =========================== //

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

// ====================   MoveArray   =========================== //

MoveArray::MoveArray () {
    array = static_cast<Move*>(malloc(MAX_SIZE * sizeof(Move)));
}

MoveArray::~MoveArray () {
    delete array;
}

Move MoveArray::operator[] (size_t index) {
    if (index + 1 > size) return NULL_MOVE;
    return array[index];
}

Move MoveArray::At (size_t index) {
    if (index + 1 > size) return NULL_MOVE;
    return array[index];
}

size_t MoveArray::Size () {
    return size;
}

size_t MoveArray::NonNullSize () {
    return non_null_size;
}

void MoveArray::AddMove (Move m) {
    array[size] = m;
    size++;
    non_null_size++;
}

void MoveArray::NullifyMove (size_t index) {
    array[index] = NULL_MOVE;
    non_null_size--;
}

void MoveArray::Trim () {
    array = static_cast<Move*>(realloc(array, size * sizeof(Move)));
}

void MoveArray::Clear () {
    size = 0;
    non_null_size = 0;
}

bool MoveArray::NoLegalMoves () {
    return non_null_size == 0;
}
