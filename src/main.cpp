// TODO:
//      Make object files for faster building of project 

#include <iostream>

#include "SDL2/SDL.h"
#include "board.hpp"
#include "GUI.cpp"

const int framerate = 30;
const int target_delta = 1000 / framerate;
Uint32 start_frame;
int frame_delta;

Board board;
char position[90] = "X"; // "X" means startpos
bool running = true;

void HandleEvents () {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            GUI::Click(event.button.x, event.button.y);
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_4) {
                GUI::ExecutePromotion(event.key.keysym.sym - SDLK_1);
            } else if (event.key.keysym.sym == SDLK_LEFT) {
                GUI::UndoUserMove();
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
    GUI::Init(&board);
    running = true;

    if (position[0] == 'X')
         board.RestartBoard();
    else board.RestartBoard(position);

    board.PrintBoard();
    GUI::FillHighlightMatrix();

    while (running) {
        start_frame = SDL_GetTicks();

        HandleEvents();
        GUI::GUI();

        frame_delta = SDL_GetTicks();
        if (frame_delta < target_delta) SDL_Delay(target_delta - frame_delta);
    }

    GUI::Shutdown();
}

uint64_t Perft (int depth) {
    std::vector<Move> move_list;
    size_t n_moves, i;
    uint64_t nodes = 0;

    if (depth == 0) 
        return 1ULL;

    move_list = board.GetLegalMoves();
    n_moves = move_list.size();
    for (i = 0; i < n_moves; i++) {
        board.ExecuteMove(move_list[i]);
        // GUI::GUI();
        // SDL_Delay(100);
        nodes += Perft(depth - 1);
        board.UndoMove(move_list[i]);
    }

    return nodes;
}

void StartPerft (int max_depth) {
    for (int i = 1; i <= max_depth; ++i) {
        board.RestartBoard();
        printf("Nodes reached at %d ply: %llu\n", i, Perft(i)); 
    }
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
            } else if (command == "perft") {
                std::cin >> argument;
                StartPerft(HelperClass::GetNumberFromPointer(argument.c_str()));
            } else if (command == "position") {
                ChangePosition();
            } else if (command == "help") {
                // TODO: make help function
                // PrintHelp();
            } else {
                printf("Unknown command\n");
            }
        }
    }

    return 0;
}