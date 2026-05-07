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

constexpr int BOARDROWS = 8;
constexpr int BOARDCOLS = 8;
constexpr int pieceNumber = 16;

class Piece; 
void printBoard(Piece* b1[BOARDROWS][BOARDCOLS]);