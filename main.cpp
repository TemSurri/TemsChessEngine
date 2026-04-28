// main.cpp : Defines the entry point for the application.
//

#include "piecetype.h"
#include "piece.cpp"

using namespace std;

const int BOARDROWS = 8;
const int BOARDCOLS = 8;

// will implcicityly convert to int

int board[BOARDCOLS][BOARDROWS]{};

// this function will initialize the board with the pieces int their standard starting position
constexpr void initBoard(int b1[BOARDROWS][BOARDCOLS]) {

	for (int r{}; r < BOARDROWS; r++) {

		for (int c{}; c < BOARDCOLS; c++) {

			if (r == 1 || r == 6) {
				b1[r][c] = Pawn;
			}

			else if (r == 0 || r == 7) {
				if (c == 0 || c == 7) {
					b1[r][c] = Rook;
				}
				else if (c == 1 || c == 6) {
					b1[r][c] = Knight;
				}
				else if (c == 2 || c == 5) {
					b1[r][c] = Bishop;
				}
				else if (c == 3) {
					b1[r][c] = Queen;
				}
				else if (c == 4) {
					b1[r][c] = King;
				};
			};
		};
	};
};

void printBoard(int b1[BOARDROWS][BOARDCOLS]) {
	for (int r{}; r < BOARDROWS; r++) {
		for (int c{}; c < BOARDCOLS; c++) {
			cout << b1[r][c] << " ";
		};
		cout << endl;
	};
};

int main(){

	initBoard(board);
	printBoard(board);
	return 0;
	
};

