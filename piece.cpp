#include "pieceInfo.h"
#include <iostream>
#include <vector>
#include "gameInfo.h"
#include <array>
#include <cstdlib>

vector<std::array<int, 2>> Piece::oneStep() {

	vector< std::array<int, 2>> theoretical_moves;
	theoretical_moves.reserve(8);
	// will put it in the order of up(0), down, right, left, upper-right, upper-left, lower-right, lower-left
	theoretical_moves.push_back({ r - 1, c });
	theoretical_moves.push_back({ r + 1, c });
	theoretical_moves.push_back({ r, c + 1});
	theoretical_moves.push_back({ r, c - 1});
	theoretical_moves.push_back({ r - 1, c + 1 });
	theoretical_moves.push_back({ r - 1, c - 1 });
	theoretical_moves.push_back({ r + 1, c + 1 });
	theoretical_moves.push_back({ r + 1, c - 1});


	return theoretical_moves;

};

vector< std::array<int, 2>> Piece::verticalMovement() {
	int lowerbound = BOARDROWS;
	int upperbound = {};

	vector< std::array<int, 2>> theoretical_moves;

	for (int i{ 1 }; i < abs(r - lowerbound); i++) {
		if (board[r + i][c]) {

			if (board[r + i][c]->is_white == is_white) {
				break;
			}
			else if (board[r + i][c]->is_white != is_white) {
				theoretical_moves.push_back({ r + i, c });
				break;
			}

		}
		theoretical_moves.push_back({ r + i, c });
	}

	for (int i{ 1 }; i < abs(r - upperbound) + 1; i++) {
		if (board[r - i][c]) {

			if (board[r - i][c]->is_white == is_white) {
				break;
			}
			else if (board[r - i][c]->is_white != is_white) {
				theoretical_moves.push_back({ r - i, c });
				break;
			}

		}
		theoretical_moves.push_back({ r - i, c });
	}

	return theoretical_moves;

};

vector< std::array<int, 2>> Piece::horizontalMovement() {
	
	int rightbound = BOARDCOLS;
	int leftbound = 0;


	vector< std::array<int, 2>> theoretical_moves;

	
	for (int i{1}; i < abs(c-rightbound); i++) {

		if (board[r][c+i]) {

			if (board[r][c +i ]->is_white == is_white) {
				break;
			}
			else if (board[r ][c + i]->is_white != is_white) {
				theoretical_moves.push_back({ r , c + i});
				break;
			}

		}
		theoretical_moves.push_back({ r, c+i });
	}

	for (int i{1}; i < abs(c-leftbound)+1; i++) {
		if (board[r][c - i]) {

			if (board[r][c - i]->is_white == is_white) {
				break;
			}
			else if (board[r][c - i]->is_white != is_white) {
				theoretical_moves.push_back({ r , c - i });
				break;
			}

		}
		theoretical_moves.push_back({ r, c-i });
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
	int iterator{};

	if (abs(upperbound-r) > abs(rightbound-c)) {
		iterator = abs(rightbound - c);
	}
	else {
		iterator = abs(upperbound - r);
	}

	for (int i{ 1 }; i < iterator + 1; i++) {

		//checking occupany
		if (board[r - i][c + i]) {

			if (board[r - i][c + i]->is_white == is_white) {
				break;
			}
			else if (board[r - i][c + i]->is_white != is_white) {
				theoretical_moves.push_back({ r - i, c + i });
				break;
			}
		}

		theoretical_moves.push_back({ r - i, c + i });
	}

	//top left
	if (abs(upperbound - r) > abs(leftbound - c)) {
		iterator = abs(leftbound - c);
	}
	else {
		iterator = abs(upperbound - r);
	}

	for (int i{ 1 }; i < iterator + 1; i++) {

		//checking occupany
		if (board[r - i][c - i]) {

			if (board[r - i][c - i]->is_white == is_white) {
				break;
			}
			else if (board[r - i][c - i]->is_white != is_white) {
				theoretical_moves.push_back({ r - i, c - i });
				break;
			}
		}

		theoretical_moves.push_back({ r - i, c - i });
	}
	
	//bottom right 
	if (abs(lowerbound - r) > abs(rightbound - c)) {
		iterator = abs(rightbound - c);
	}
	else {
		iterator = abs(lowerbound - r);
	}

	for (int i{ 1 }; i <iterator + 1; i++) {


		//checking occupany
		if (board[r + i][c + i]) {

			if (board[r + i][c + i]->is_white == is_white) {
				break;
			}
			else if (board[r + i][c + i]->is_white != is_white) {
				theoretical_moves.push_back({ r + i, c + i });
				break;
			}
		}

		theoretical_moves.push_back({ r + i, c + i });
	}
	
	//bottom left 
	if (abs(lowerbound - r) > abs(leftbound - c)) {
		iterator = abs(leftbound - c);
	}
	else {
		iterator = abs(lowerbound - r);
	}

	for (int i{ 1 }; i < iterator + 1; i++) {

		//checking occupany
		if (board[r + i][c - i]) {

			if (board[r + i][c - i]->is_white == is_white) {
				break;
			}
			else if (board[r + i][c - i]->is_white != is_white) {
				theoretical_moves.push_back({ r + i, c - i });
				break;
			}
		}

		theoretical_moves.push_back({ r + i, c - i });
	}


	return theoretical_moves;
	
};



