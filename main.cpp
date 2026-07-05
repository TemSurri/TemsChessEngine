// main.cpp : Defines the entry point for the application.
//

#include "game.h"



int main() {
    ClassicChess g;
    
    g.gameLoopVSminimaxAI(true, 5); 

    return 0;
}

