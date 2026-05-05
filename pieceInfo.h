#include <iostream>
#include <vector>
#include "gameInfo.h"

using namespace std;

class Piece {
private:
	int r{};
	int c{};
	bool is_white;
	int type;
public:
	Piece(int x, int y, bool white, PieceType type)
		: r{ x }, c{ y }, is_white{ white }, type{ type } {
	};

	void move(int x, int y) {
		this->r = x;
		this->c = y;
	};

	void changeType(PieceType type) {
		type = type;
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
		std::cout << type<<endl << "Row : " << r << endl << "Col : " << c << endl;
		return;
	}

	vector< std::array<int, 2>> horizontalMovement();
	vector< std::array<int, 2>> verticalMovement();
	vector< std::array<int, 2>> diagonalMovement();
	vector< std::array<int, 2>>oneStep();

};
// map of board with pointers to the pieces for quick search up
extern Piece* board[BOARDROWS][BOARDCOLS];

// list of pieces to keep track of
extern std::vector<Piece> whitePieces;
extern std::vector<Piece> blackPieces;