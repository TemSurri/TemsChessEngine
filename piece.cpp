#include "pieceInfo.h"
#include <iostream>
#include <vector>
#include "gameInfo.h"
#include <array>
#include <cstdlib>
#include <iterator>
vector<std::array<int, 2>> Piece::oneStep() {

	vector< std::array<int, 2>> straight_steps;
	vector< std::array<int, 2>> diag_steps;

	vector< std::array<int, 2>> theoretical_moves;


	if (type == PieceType::King) {
		// will put it in the order of up(0), down, right, left, 
		straight_steps.push_back({ r - 1, c });
		straight_steps.push_back({ r + 1, c });
		straight_steps.push_back({ r, c + 1 });
		straight_steps.push_back({ r, c - 1 });

		// upper-right, upper-left, lower-right, lower-left
		diag_steps.push_back({ r - 1, c + 1 });
		diag_steps.push_back({ r - 1, c - 1 });
		diag_steps.push_back({ r + 1, c + 1 });
		diag_steps.push_back({ r + 1, c - 1 });
	}
		
	else if (type == PieceType::Pawn){

		if (upper) {
			straight_steps.push_back({ r + 1, c });
			diag_steps.push_back({ r + 1, c + 1 });
			diag_steps.push_back({ r + 1, c - 1 });
			if (moves == 0) {
				straight_steps.push_back({ r + 2, c });
			}
		}
		else {
			straight_steps.push_back({ r - 1, c });
			diag_steps.push_back({ r - 1, c + 1 });
			diag_steps.push_back({ r - 1, c - 1 });
			if (moves == 0) {
				straight_steps.push_back({ r - 2, c });
			}
		}
	}

	//this is where i can remove the outlier parts

	
	for (int i{}; i < straight_steps.size(); i++) {
		int r = straight_steps[i][0];
		int c = straight_steps[i][1];

		if (r < 0 || c < 0) {
			continue;
		};

		if (r > BOARDROWS || c > BOARDCOLS) {
			continue;
		};

		if (board[r][c]) {
			if (type == PieceType::Pawn) {
				continue;
			}
			if (board[r][c]->is_white == is_white) {
				continue;
			}
		}

		theoretical_moves.push_back({ r,c });
	}

	for (int i{}; i < diag_steps.size(); i++) {
		int r = diag_steps[i][0];
		int c = diag_steps[i][1];

		if (r < 0 || c < 0) {
			continue;
		};

		if (r > BOARDROWS || c > BOARDCOLS) {
			continue;
		};
	
		if (board[r][c]) {

			if (board[r][c]->is_white == is_white) {
				continue;
			}

		}
		else if (type == PieceType::Pawn) {
			continue;
		}
		theoretical_moves.push_back({ r,c });
	}
	
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

vector< std::array<int, 2>> Piece::pseudoLegalMoves() {

	switch (type) {

	case(PieceType::Bishop):
		return this->diagonalMovement();

	case(PieceType::Pawn):
		return this->oneStep();

	case(PieceType::King):
		return this->oneStep();

	case(PieceType::Queen): {
		vector< std::array<int, 2>> a = this->verticalMovement();
		vector< std::array<int, 2>> b = this->horizontalMovement();
		vector< std::array<int, 2>> c = this->diagonalMovement();

		a.insert(a.end(), b.begin(), b.end());
		a.insert(a.end(), c.begin(), c.end());

		return a;
	}

	case(PieceType::Rook): {

		vector<std::array<int, 2>> a = this->verticalMovement();
		vector<std::array<int, 2>> b = this->horizontalMovement();

		a.insert(a.end(), b.begin(), b.end());

		return a;
	}
	case(PieceType::Knight): {
		vector<std::array<int, 2>> a;
		return a;
		}
	}
}