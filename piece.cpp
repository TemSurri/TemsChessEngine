#include "pieceInfo.h"
#include <iostream>
#include <vector>
#include "gameInfo.h"
#include <array>
#include <cstdlib>


vector< std::array<int, 2>> Piece::verticalMovement() {
	int lowerbound = BOARDROWS;

	vector< std::array<int, 2>> theoretical_moves;

	//total column
	for (int i{}; i < lowerbound; i++) {
		if (i == r) {
			continue;
		}
		theoretical_moves.push_back({ i, c });
	}

	return theoretical_moves;

};

vector< std::array<int, 2>> Piece::horizontalMovement() {
	
	int lowerbound = BOARDROWS;

	vector< std::array<int, 2>> theoretical_moves;

	// total row
	for (int i{}; i < lowerbound; i++) {
		if (i == c) {
			continue;
		}
		theoretical_moves.push_back({ r, i });
	}

	return theoretical_moves;

};


vector< std::array<int, 2>> Piece::diagonalMovement() {

	int upperbound = 0;
	int lowerbound = BOARDROWS-1;

	int leftbound = 0;
	int rightbound = BOARDCOLS-1;

	vector< std::array<int, 2>> theoretical_moves;
	
	//top right
	if (abs(upperbound-r) > abs(rightbound-c)) {
		for (int i{ 1 }; i < abs(rightbound - c) + 1; i++) {
			theoretical_moves.push_back({ r-i, c+i });
		}
	}
	else {
		for (int i{ 1 }; i < abs(upperbound - r) + 1; i++) {
			theoretical_moves.push_back({ r-i, c+i });
		}

	}
	
	//top left
	if (abs(upperbound - r) > abs(leftbound - c)) {
		for (int i{ 1 }; i < abs(leftbound - c) + 1; i++) {
			theoretical_moves.push_back({ r - i, c - i });
		}
	}
	else {
		for (int i{ 1 }; i < abs(upperbound - r) + 1; i++) {
			theoretical_moves.push_back({ r - i, c - i });
		}
	}
	
	//bottom right 
	if (abs(lowerbound - r) > abs(rightbound - c)) {
		for (int i{ 1 }; i < abs(rightbound - c) + 1; i++) {
			theoretical_moves.push_back({ r + i, c + i });
		}
	}
	else {
		for (int i{ 1 }; i < abs(lowerbound - r) + 1; i++) {
			theoretical_moves.push_back({ r + i, c + i });
		}

	}
	
	//bottom left 
	if (abs(lowerbound - r) > abs(leftbound - c)) {
		for (int i{ 1 }; i < abs(leftbound - c) + 1; i++) {
			theoretical_moves.push_back({ r + i, c - i });
		}
	}
	else {
		for (int i{ 1 }; i < abs(lowerbound - r) + 1; i++) {
			theoretical_moves.push_back({ r + i, c - i });
		}

	}

	return theoretical_moves;
	
};



