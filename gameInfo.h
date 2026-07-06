// CMakeProject1.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <array>



// TODO: Reference additional headers your program requires here.

enum PieceType {

	Empty,
	Pawn,
	Bishop,
	Knight,
	Rook,
	Queen,
	King,

};


enum MoveFashion : int {
	STANDARD,
	CASTLE,
	PAWN_PROMOTION,
	EN_PASSENT

};

class Piece;

struct MoveEndpoint {

	int r;
	int c;
	MoveFashion fashion;
	int value;
	Piece* p;

};

constexpr int BOARDROWS = 8;
constexpr int BOARDCOLS = 8;
constexpr int pieceNumber = 16;
