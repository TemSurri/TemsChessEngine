#include <iostream>
#include "piecetype.h"
using namespace std;

class Piece {
private :
	int x{};
	int y{};
	bool is_white;
	int type;
public : 
	Piece(int x, int y, bool white, PieceType type)
		: x{ x }, y{ y }, is_white{ white }, type{ type } {
	};

	void move(int x, int y) {
		this->y = y;
		this->x = x;
	};



};