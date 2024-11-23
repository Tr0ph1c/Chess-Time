#include "board.hpp"
#include "piece.hpp"
#include "move.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

// TODO:
// 50 move rule

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
    move_set = GetLegalMoves();
}

void Board::EndTurn () {
    // Stop clock (not necessarily)

    // Check if its stalemate or checkmate or draw by other means
    if (fifty_move_rule == 50) {
        // Game is a draw.
        printf("Game is draw by 50 move rule.\n");
    }
    
    move_set = GetLegalMoves();
}

std::vector<Move> Board::GetAllMoves () {
    std::vector<Move> moves;

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
            uint8_t change_castling_rights = PR_CAS;

            if (_raw == ROOK) diri_max = 4;
            else if (_raw == BISHOP) diri = 4;

            if (_raw == ROOK && castling_rights) {
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
                        moves.push_back(CreateMove(start_square, next_square, QUIET_MOVE, change_castling_rights));
                    } else if (IsAllyPiece(squares[next_square])) {
                        break;
                    } else if (IsEnemyPiece(squares[next_square])) {
                        moves.push_back(CreateMove(start_square, next_square, CAPTURE_MOVE, change_castling_rights, squares[next_square]));
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
                            moves.push_back(CreateMove(start_square, next_square, QUIET_MOVE));
                        } else if (IsAllyPiece(squares[next_square])) {
                            continue;
                        } else if (IsEnemyPiece(squares[next_square])) {
                            moves.push_back(CreateMove(start_square, next_square, CAPTURE_MOVE, PR_CAS, squares[next_square]));
                        }
                    }
                }
            }
        } else if (_raw == KING) {
            uint8_t change_castle_rights = (IsWhiteToPlay())? 0b0011 : 0b1100;

            for (int dir = 0; dir < 8; ++dir) {
                int next_square = start_square + directions[dir];
                int next_file = next_square % 8;

                if (next_square >= 0 && next_square < 64) {
                    if (abs(start_file - next_file) < 2) {
                        if (squares[next_square] == EMPTY) {
                            moves.push_back(CreateMove(start_square, next_square, QUIET_MOVE, change_castle_rights));
                        } else if (IsAllyPiece(squares[next_square])) {
                            continue;
                        } else if (IsEnemyPiece(squares[next_square])) {
                            moves.push_back(CreateMove(start_square, next_square, CAPTURE_MOVE, change_castle_rights, squares[next_square]));
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
                    if (IsSquareAttacked(squares[WKSC_SQUARE] - 1)) K = false;
                    if (K) moves.push_back(CreateMove(start_square, WKSC_SQUARE, CASTLE_KS, change_castle_rights));
                }
                if (castling_rights & 0b0100) {
                    for (uint8_t s : white_queenside) {
                        if (squares[s] == EMPTY) continue;
                        Q = false;
                        break;
                    }
                    if (IsSquareAttacked(squares[WQSC_SQUARE] + 1)) Q = false;
                    if (Q) moves.push_back(CreateMove(start_square, WQSC_SQUARE, CASTLE_QS, change_castle_rights));
                }
            } else {
                if (castling_rights & 0b0010) {
                    for (uint8_t s : black_kingside) {
                        if (squares[s] == EMPTY) continue;
                        K = false;
                        break;
                    }
                    if (IsSquareAttacked(squares[BKSC_SQUARE] - 1)) K = false;
                    if (K) moves.push_back(CreateMove(start_square, BKSC_SQUARE, CASTLE_KS, change_castle_rights));
                }
                if (castling_rights & 0b0001) {
                    for (uint8_t s : black_queenside) {
                        if (squares[s] == EMPTY) continue;
                        Q = false;
                        break;
                    }
                    if (IsSquareAttacked(squares[BQSC_SQUARE] + 1)) Q = false;
                    if (Q) moves.push_back(CreateMove(start_square, BQSC_SQUARE, CASTLE_QS, change_castle_rights));
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
                    moves.push_back(CreateMove(start_square, forward_square, PROMO_QUEEN));
                    moves.push_back(CreateMove(start_square, forward_square, PROMO_BISHOP));
                    moves.push_back(CreateMove(start_square, forward_square, PROMO_KNIGHT));
                    moves.push_back(CreateMove(start_square, forward_square, PROMO_ROOK));
                } else {
                    moves.push_back(CreateMove(start_square, forward_square, QUIET_MOVE));
                    if (is_first_move && squares[start_square + dir*16] == EMPTY)
                        moves.push_back(CreateMove(start_square, start_square + dir*16, DOUBLE_PAWN));
                }
            }

            int first_diagonal = start_square + dir*7;
            int second_diagonal = start_square + dir*9;
            if (IsEnemyPiece(squares[first_diagonal]) &&
            abs(start_file - first_diagonal%8) == 1) {
                if (is_next_rank_promo) {
                    moves.push_back(CreateMove(start_square, first_diagonal, CAPTURE_MOVE | PROMO_QUEEN, PR_CAS, squares[first_diagonal]));
                    moves.push_back(CreateMove(start_square, first_diagonal, CAPTURE_MOVE | PROMO_BISHOP, PR_CAS, squares[first_diagonal]));
                    moves.push_back(CreateMove(start_square, first_diagonal, CAPTURE_MOVE | PROMO_KNIGHT, PR_CAS, squares[first_diagonal]));
                    moves.push_back(CreateMove(start_square, first_diagonal, CAPTURE_MOVE | PROMO_ROOK, PR_CAS, squares[first_diagonal]));
                } else {
                    moves.push_back(CreateMove(start_square, first_diagonal, CAPTURE_MOVE, PR_CAS, squares[first_diagonal]));
                }
            }
            if (IsEnemyPiece(squares[second_diagonal]) &&
            abs(start_file - second_diagonal%8) == 1) {
                if (is_next_rank_promo) {
                    moves.push_back(CreateMove(start_square, second_diagonal, CAPTURE_MOVE | PROMO_QUEEN, PR_CAS, squares[second_diagonal]));
                    moves.push_back(CreateMove(start_square, second_diagonal, CAPTURE_MOVE | PROMO_BISHOP, PR_CAS, squares[second_diagonal]));
                    moves.push_back(CreateMove(start_square, second_diagonal, CAPTURE_MOVE | PROMO_KNIGHT, PR_CAS, squares[second_diagonal]));
                    moves.push_back(CreateMove(start_square, second_diagonal, CAPTURE_MOVE | PROMO_ROOK, PR_CAS, squares[second_diagonal]));
                } else {
                    moves.push_back(CreateMove(start_square, second_diagonal, CAPTURE_MOVE, PR_CAS, squares[second_diagonal]));
                }
            }

            // en passant
            if (EnPassantExists()) {
                if (first_diagonal == enpassant_place &&
                abs(start_file - first_diagonal%8) == 1) {
                    moves.push_back(CreateMove(start_square, first_diagonal, EN_PASSANT, PR_CAS, PAWN));
                } else if(second_diagonal == enpassant_place &&
                abs(start_file - second_diagonal%8) == 1) {
                    moves.push_back(CreateMove(start_square, second_diagonal, EN_PASSANT, PR_CAS, PAWN));
                }
            }
        }
    }

    return moves;
}

// TODO: Optimize by applying pins and
// keeping track of attacked squares
std::vector<Move> Board::GetLegalMoves () {
    std::vector<Move> moves = GetAllMoves();

    for (size_t i = 0; i < moves.size(); ++i) {
        ExecuteMove(moves[i]);
        color_to_play = SwitchColor(color_to_play);
        if (IsInCheck()) {
            moves[i] = NULL_MOVE;
        }
        color_to_play = SwitchColor(color_to_play);
        UndoMove(tracker.GetCurrMove());
    }

    return moves;
}

bool Board::IsInCheck () {
    return IsWhiteToPlay()? IsSquareAttacked(white_king_pos) : IsSquareAttacked(black_king_pos);
}

bool Board::IsSquareAttacked (int square_index) {
    Piece opp_color = SwitchColor(color_to_play);

    for (int m : knight_moves) {
        if (squares[square_index + m] == (KNIGHT | opp_color)) return true;
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
                }
            }
        }
    }

    return false;
}

void Board::ExecuteMove (Move move) {
    if (IsNullMove(move)) return;

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

    if (!IsDoublePawn(move)) enpassant_place = -1;

    if (IsCapture(move) || RawPiece(moved_piece) == PAWN) {
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
    castling_rights |= ~GetCastleRights(move);

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
