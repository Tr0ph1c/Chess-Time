#pragma once

#include <board.hpp>
#include <list>
#include <string>


enum BoardStateFlag{
    NON                      = 0b00000000 , // Nothing
    KWR_MOVED                = 0b00000001 , // King White Rook Moved
    WK_MOVED                 = 0b00000010 , // White King Moved
    QWR_MOVED                = 0b00000100 , // Qween White Rook Moved
    W_EN_PASSANT_AVAILABLE   = 0b00001000 , // En Passant is Available For White 
    KBR_MOVED                = 0b00010000 , // King Black Rook
    BK_MOVED                 = 0b00100000 , // Black King Moved
    QBR_MOVED                = 0b01000000 , // Qween Black Rook
    B_EN_PASSANT_AVAILABLE   = 0b10000000 , // En Passant is Available For White 
};


typedef struct {
    Move move;
    Piece tacken_piece;
    uint8_t state_summary ;
}TrackerMove;

 
class GameTracker
{
private:
    std::list<TrackerMove> Moves;
    std::list<TrackerMove>::iterator curr_pos;
    uint8_t init_state_summary;
    std::string start_pos; //FEN
    Board *board;

public:
    GameTracker(Board *board);
    GameTracker(Board *board, std::string FEN);
    GameTracker();

    size_t MovesCount();
    bool IsEmpty();

    void ImportPGN(std::string);
    std::string ExportPGN();
    void SetFEN(std::string  FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" );
    // void SetFEN();

    // Moves stuff
    void Prev();
    void Next();
    /*
    * GoToMove change the board state to MoveNumber state
    * MoveNumber must be an intger value which (MoveNumber >= 0 && MoveNumber < MovesCount)
    */
    void GoToMove(int MoveNumber);
    bool IsThisLastMove();
    bool IsThisFristMove();
    /*
    * NewMove Inset new move 
    * Pre : This must be the last move played
    */
    void NewMove(Move mv ,Piece taken_piece, BoardStateFlag state_flag);
};
