# How to Build
- Use makefile
    ```
    cd Chess-Time
    make
    ```
- Make sure you have the dependencies in [dependencies](#dependencies) :warning:

# Dependencies
- [SDL2](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.8)
- [SDL_image](https://github.com/libsdl-org/SDL_image/releases/tag/release-2.8.2)
- [SDL_ttf](https://github.com/libsdl-org/SDL_ttf/releases/tag/release-2.22.0)

# How to use
* After building the project, run the `chesstime` executable.
- CLI:
    * `exit`             : exits the program.
    * `position [FEN/X]` : sets the `position` to `[FEN]`, only accepts valid [FEN strings](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation) OR "X" for the standard starting chess position.
    * `start`            : starts a GUI board with the position from `position`.
    * `startai`          : starts a game against an AI adversary.
    * `perft [depth]`    : starts an iterative [perft test](https://www.chessprogramming.org/Perft) starting from `position` till `[depth]` and prints the results.
    * `divide [depth]`   : outputs the perft result for the sub-tree of each move in the current position. 
- GUI:
    * Any click can be initiated using mouse0 or mouse1.
    * Click to select a piece, click on a square to move the selected piece to the clicked square.
    * Press the "Back arrow" key on the keyboard to irreversibly undo the last move.
