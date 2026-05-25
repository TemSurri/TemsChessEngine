#pragma once
#include <vector>
#include "pieceInfo.h"
#include <array>

class ClassicChess {


	struct BoardState {
		bool draw = false;

		bool whiteCheckMated = false;
		bool blackCheckMated = false;

		bool whiteChecked = false;
		bool blackChecked = false;

		bool normal = false;
	};

	struct MoveInfo {
		
		Piece* piece;
		std::array<std::array<int, 2>, 2> move;

	};

	private:
		BoardState game;
		bool white_upper = true;

		int iterator{};

		Piece* board[BOARDROWS][BOARDCOLS]{nullptr};

		Piece* blackKing{ nullptr };
		Piece* whiteKing{ nullptr };

		std::vector<MoveInfo> blackMoves{};
		std::vector<MoveInfo> whiteMoves{};

		

	public:
		//public for now 

		std::vector<Piece> whitePieces;
		std::vector<Piece> blackPieces;
		//----------------

		ClassicChess() {
			
		};

		//helper functions
		void printBoard() {
			for (int r{}; r < BOARDROWS; r++) {
				for (int c{}; c < BOARDCOLS; c++) {

					if (board[r][c] == nullptr) {
						cout << 0 << " ";
					}
					else {
						cout << board[r][c]->getType() << " ";
					}
				};
				cout << endl;

			};
		};

		//move for testing and debuggin only
		void move(int ogR, int ogC, int newR, int newC) {
			Piece* p = board[ogR][ogC];
			p->toString();
			board[newR][newC] = p;
			p->move(newR, newC);
			p->toString();

			board[ogR][ogC] = nullptr;
			// auto empties old spot isnt accurate for castling
		};

		void move(MoveInfo move) {

			auto start = move.move[0];
			auto end = move.move[1];

			//piece info update
			move.piece->move(end[0], end[1]);

			//board update
			move.piece = board[end[0]][end[1]];
			
			// make start empty, obv diff for castling
			board[start[0]][start[1]] = nullptr;

		};


		void printMoves(vector<array<int, 2>> list) {

			for (int i{}; i < list.size(); i++) {
				cout << "(" << list[i][0] << ", " << list[i][1] << ") " << endl;
			}
		}

		void printAllMoves() {

			std::cout << "White Moves"<<std::endl;
			for (MoveInfo move: whiteMoves) {

				auto start = move.move[0];
				auto end = move.move[1];
	
				std::cout << "Piece("<<move.piece->getType()<<"):   " << '(' << start[0] << ", " << start[1] << ") -->" << '(' << end[0] << ", " << end[1] << ')' << endl;

			}

			std::cout << "Black Moves"<<std::endl;
			for (MoveInfo move : blackMoves) {

				auto start = move.move[0];
				auto end = move.move[1];

				std::cout << "Piece(" << move.piece->getType() << "):   " << '(' << start[0] << ", " << start[1] << ") -->" << '(' << end[0] << ", " << end[1] << ')' << endl;

			}

		}

		Piece* storePiece(int r, int c, PieceType type);
		void generateLegalMoves();

		void initClassicGame();

		BoardState virtualMove(MoveInfo move);
		BoardState calculateState();
		BoardState gameLoop();
		BoardState move( bool white );


		// everything hsould end up private escpet the final startGame()


};