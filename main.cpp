// main.cpp : Defines the entry point for the application.
//

#include "gameInfo.h"
#include "pieceInfo.h"
#include <vector>
#include <array>

using namespace std;

Piece* board[BOARDROWS][BOARDCOLS]{ nullptr }; //change this to board with objects
std::vector<Piece> whitePieces;
std::vector<Piece> blackPieces;

//helper function to store each piece in it's vector and return a pointer to that location
Piece* storePiece(int r, int c, PieceType type) {

	// assume row greater than 4 is white
	// this is where we can store differnt objects based on type but for not itll be on Piece class later can do inhertiance

	//upper is white
	bool is_white = (r > 4) ? false : true;

	Piece piece = Piece(r, c, is_white, type);
	
	if (is_white) {
		whitePieces.push_back(piece);
		return &(whitePieces.back());
	}
	else {
		blackPieces.push_back(piece);
		return &(blackPieces.back());
	}
};

// this function will initialize the board with the pieces int their standard starting position
void initBoardAndVectors(Piece* b1[BOARDROWS][BOARDCOLS]) {
	whitePieces.reserve(pieceNumber);
	blackPieces.reserve(pieceNumber);

	for (int r{}; r < BOARDROWS; r++) {

		for (int c{}; c < BOARDCOLS; c++) {

			if (r == 1 || r == 6) {
				b1[r][c] = storePiece(r, c, Pawn);
			}
			else if (r == 0 || r == 7) {
				if (c == 0 || c == 7) {
					b1[r][c] = storePiece(r, c, Rook);
				}
				else if (c == 1 || c == 6) {
					b1[r][c] = storePiece(r, c, Knight);
				}
				else if (c == 2 || c == 5) {
					b1[r][c] = storePiece(r, c, Bishop);
				}
				else if (c == 3) {
					b1[r][c] = storePiece(r, c, Queen);
				}
				else if (c == 4) {
					b1[r][c] = storePiece(r, c, King);
				}
			}
		};
	};
};

//test move sequence
void move(int ogR, int ogC, int newR, int newC) {
	Piece* p = board[ogR][ogC];
	p->toString();
	board[newR][newC] = p;
	p->move(newR, newC);
	p->toString();
	
	board[ogR][ogC] = nullptr;
	// auto empties old spot isnt accurate for castling
}

void printBoard(Piece* b1[BOARDROWS][BOARDCOLS]) {
	for (int r{}; r < BOARDROWS; r++) {
		for (int c{}; c < BOARDCOLS; c++) {

			if (b1[r][c] == nullptr) {
				cout << 0 << " ";
			}
			else {
				cout << b1[r][c]->getType() << " ";
			}
		};
		cout << endl;
		
	};
};

void printMoves(vector<array<int,2>> list) {
	
	for (int i{}; i < list.size(); i++) {
		cout << "(" << list[i][0] << ", " << list[i][1] << ") " << endl;
	}
}

int main(){

	bool game = true;

	
	initBoardAndVectors(board);

	whitePieces[4].toString();
	printMoves(whitePieces[4].pseudoLegalMoves());
	move(0, 3, 2, 3);
	
	printMoves(whitePieces[4].pseudoLegalMoves());
	printBoard(board);

	while (game) {

	}
	return 0;
	
};

