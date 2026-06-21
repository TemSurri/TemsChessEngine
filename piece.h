#include <iostream>
#include <vector>
#include "gameInfo.h"
#include <array>
#pragma once

using namespace std;

class Piece {
private:
	int r{};
	int c{};
	bool is_white;
	int type;
	bool upper = is_white;
	int moves{};
	

	vector< std::array<int, 2>> horizontalMovement();
	vector< std::array<int, 2>> verticalMovement();
	vector< std::array<int, 2>> diagonalMovement();
	vector< std::array<int, 2>> oneStep();
	vector< std::array<int, 2>> horseMovement();

	Piece* (&board)[BOARDROWS][BOARDCOLS];

public:
	bool captured = false;

	Piece(int x, int y, bool white, PieceType type, Piece* (&board)[BOARDROWS][BOARDCOLS])
		: r{ x }, c{ y }, is_white{ white }, type{ type }, board{ board } {
	};

	void move(int x, int y) {
		this->r = x;
		this->c = y;
	};

	void changeType(PieceType t) {
		this->type = t;
	}

	int getType() {
		return type;
	}

	bool getColor() {
		return is_white;
	}

	int getCol() {
		return c;
	}

	int getRow() {
		return r;
	}

	void toString() {
		std::cout << type<<endl << "Row : " << r << endl << "Col : " << c << endl << is_white << endl;
		return;
	}

	void incrementMove() {
		moves++;
	}

	void deincrementMove() {
		moves--;
	}

	int getTimesMoved() {
		return moves;
	}

	// will be dependant on the type
	vector< std::array<int, 2>> pseudoLegalMoves();
	vector< std::array<int, 2>> getCastledMoves();
	vector< std::array<int, 2>> getEnPassent();

};
