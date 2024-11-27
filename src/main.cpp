// TODO:
//      * Make object files for faster building of project
//      * Make a helper function that takes a board index and transforms it to UCI notation ie c6
//      * Add sound to GUI
//      * Introduce a Perft Suite that tests for perft results on multiple depths
//   from multiple positions and compares those against a set of known perft
//   results from https://www.chessprogramming.org/Perft_Results

#include <iostream>

#include "SDL2/SDL.h"
#include "board.hpp"
#include "GUI.hpp"
#include "AI.hpp"

GUI* GUI_instance;

Board board;
char position[90] = "X"; // "X" means startpos
char help_str[] =
        "exit             : exits the program.\n"
        "position [FEN/X] : sets the position to [FEN], only accepts valid FEN strings\n"
        "start            : starts a GUI board with the position from position.\n"
        "startai          : starts a game against an AI adversary.\n"
        "perft [depth]    : starts an iterative perft test starting from position till [depth] and prints the results.\n"
        "divide [depth]   : outputs the perft result for the sub-tree of each move in the current position. \n";

void ChangePosition () {
    std::cin.get();
    std::cin.getline(position, 90);
    printf("pos: [%s]\n", position);
    // TODO: check if FEN String is valid
    // if not switch back to "X";
}

void StartGame () {
    GUI_instance = new GUI(&board);

    if (position[0] == 'X')
         board.RestartBoard();
    else board.RestartBoard(position);

    board.PrintBoard();
    GUI_instance->FetchMoves();

    while (GUI_instance->running) {
        GUI_instance->RunGUITick();
    }

    delete GUI_instance;
}

void StartAIGame () {
    GUI_instance = new GUI(&board);
    AI computer(&board);

    if (position[0] == 'X')
         board.RestartBoard();
    else board.RestartBoard(position);

    GUI_instance->FetchMoves();

    while (GUI_instance->running) {
        if (board.IsWhiteToPlay()) {
            GUI_instance->RunGUITick();
        } else {
            Move comp_move = computer.PlayMove();
            if (comp_move != 0) {
                board.ExecuteMove(comp_move);
                GUI_instance->FetchMoves();
            } else {
                board.color_to_play = SwitchColor(board.color_to_play);
            }
        }
    }

    delete GUI_instance;
}

size_t Perft (int depth) {
    if (!depth) return 1ULL;

    SizeArray move_list;
    size_t n_moves = 0, i = 0;
    size_t nodes = 0;

    board.GetAllMoves(&move_list);
    n_moves = move_list.Size();

    if (depth == 1) 
        return n_moves;
    
    for (i = 0; i < move_list.Size(); i++) {
        board.ExecuteMove(move_list[i]);
        nodes += Perft(depth - 1);
        board.UndoMove(move_list[i]);
    }

    return nodes;
}

void StartPerft (int max_depth) {
    if (position[0] == 'X')
         board.RestartBoard();
    else board.RestartBoard(position);

    for (int i = 1; i <= max_depth; ++i) {
        printf("Nodes reached at %d ply: %llu\n", i, Perft(i)); 
    }
}

void StartDivPerft (int max_depth) {
    if (position[0] == 'X')
         board.RestartBoard();
    else board.RestartBoard(position);

    SizeArray move_list;
    board.GetAllMoves(&move_list);
    size_t combined = 0;

    for (size_t i = 0; i < move_list.Size(); ++i) {
        board.ExecuteMove(move_list[i]);
        size_t curr_perft = Perft(max_depth - 1);
        combined += curr_perft;
        printf("%s: %llu\n", GetNotationFromMove(move_list[i]).c_str(), curr_perft);
        board.UndoMove(move_list[i]);
    }

    printf("\nCombined: %llu\n", combined);
}

void PrintHelp(){
    std::cout << help_str;
}

int main (int argc, char** args) {
    while (true) {
        std::string command;
        std::string argument;

        std::cin.clear();
        std::cout << "> ";
        std::cin >> command;

        if (command.length() > 1) {
            if (command == "exit") {
                break;
            } else if (command == "start") {
                StartGame();
            } else if (command == "startai") {
                StartAIGame();
            } else if (command == "perft") {
                std::cin >> argument;
                StartPerft(HelperClass::GetNumberFromPointer(argument.c_str()));
            } else if (command == "divide") {
                std::cin >> argument;
                StartDivPerft(HelperClass::GetNumberFromPointer(argument.c_str()));
            } else if (command == "position") {
                ChangePosition();
            } else if (command == "help") {
                 PrintHelp();
            } else {
                printf("Unknown command\n");
            }
        }
    }

    return 0;
}