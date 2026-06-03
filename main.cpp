// main.cpp : Defines the entry point for the application.
//

#include "game.h"



int main(){

	ClassicChess g = ClassicChess();

	
	auto game = g.gameLoop();
	
	return 0;
	
};

