// main.cpp : Defines the entry point for the application.
//

#include "game.h"



int main(){

	ClassicChess g = ClassicChess();
	g.initClassicGame();
	g.printBoard();

	g.move(0,2,4,4);
	g.printBoard();
	
	g.generateLegalMoves();
	g.printAllMoves();

	
	
	return 0;
	
};

