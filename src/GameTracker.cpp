#include <GameTracker.hpp>



GameTracker::GameTracker(){
    curr_pos = Moves.end();
    init_state_summary = 0b00000000;
}
GameTracker::GameTracker(Board *board): GameTracker(){
    this->board = board;
    this->SetFEN();
}
GameTracker::GameTracker(Board *board , std::string FEN): GameTracker(){
    this->board = board;
    this->SetFEN(FEN);
}
void GameTracker::SetFEN(std::string FEN){
    start_pos = FEN;
    this->board->LoadBoard(FEN.c_str());
}
// void GameTracker::SetFEN(){
//     std::string FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
//     start_pos = FEN;
//     this->board->LoadBoard(FEN.c_str());
// }



size_t GameTracker::MovesCount() {
    return (Moves.size());
}
bool GameTracker::IsEmpty(){
    return (! Moves.size());
}

void GameTracker::ImportPGN(std::string PGN){

}
std::string GameTracker::ExportPGN(){
    return "NOT FINISHED";
}


void GameTracker::Prev(){

}
void GameTracker::Next(){

}
void GameTracker::GoToMove(int MoveNumber){

}
bool GameTracker::IsThisLastMove(){
    return false;
}
bool GameTracker::IsThisFristMove(){
    return false;
}
void GameTracker::NewMove(Move mv ,Piece taken_piece, BoardStateFlag state_flag){
    TrackerMove new_tracker_move;

    new_tracker_move.move = mv;
    new_tracker_move.tacken_piece = taken_piece;
    new_tracker_move.state_summary = 
        (Moves.back().state_summary & 0b01110111/*unset en passant*/) | state_flag;

    Moves.push_back(new_tracker_move);
    curr_pos = Moves.end();
    curr_pos--;
}