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

const int framerate = 30;
const int target_delta = 1000 / framerate;
Uint32 start_frame;
int frame_delta;
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


void HandleEvents () {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            GUI_instance->running = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            GUI_instance->Click(event.button.x, event.button.y);
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_4) {
                GUI_instance->ExecutePromotion(event.key.keysym.sym - SDLK_1);
            } else if (event.key.keysym.sym == SDLK_LEFT) {
                GUI_instance->UndoUserMove();
            } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                GUI_instance->running = false;
            }
            break;
        default:
            break;
    }
}

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
        start_frame = SDL_GetTicks();

        HandleEvents();
        GUI_instance->ShowFrame();

        frame_delta = SDL_GetTicks() - start_frame;
        if (frame_delta < target_delta) SDL_Delay(target_delta - frame_delta);
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
        start_frame = SDL_GetTicks();

        if (board.IsWhiteToPlay()) {
            HandleEvents();
            GUI_instance->ShowFrame();
        } else {
            Move comp_move = computer.PlayMove();
            if (comp_move != 0) {
                board.ExecuteMove(comp_move);
                GUI_instance->FetchMoves();
            } else {
                board.color_to_play = SwitchColor(board.color_to_play);
            }
        }

        frame_delta = SDL_GetTicks();
        if (frame_delta < target_delta) SDL_Delay(target_delta - frame_delta);
    }

    delete GUI_instance;
}

size_t Perft (int depth) {
    if (!depth) return 1ULL;

    MoveArray move_list;
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

    MoveArray move_list;
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