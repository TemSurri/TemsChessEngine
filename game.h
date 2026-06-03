#pragma once
#include <vector>
#include "piece.h"
#include <array>

class ClassicChess {


	struct BoardState {
		bool draw = false;

		bool whiteCheckMated = false;
		bool blackCheckMated = false;

		bool whiteChecked = false;
		bool blackChecked = false;

		bool normal = false;
		bool active = true;
	};

	enum OutCome {
		WhiteWin,
		BlackWin,
		Draw
	};

	struct MoveInfo {
		
		Piece* piece;
		std::array<std::array<int, 2>, 2> move;

	};

	private:
		BoardState game;
		bool white_upper = true;
		int iterator{};

		// to change every move
		bool white_move = true;

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

		
		void real_move(int ogR, int ogC, int newR, int newC) {
			if (board[newR][newC]) {
				board[newR][newC]->captured = true;
			}

			board[ogR][ogC]->incrementMove();
			board[ogR][ogC]->move(newR, newC);
			board[newR][newC] = board[ogR][ogC];

			board[ogR][ogC] = nullptr;
			// auto empties old spot isnt accurate for castling
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
		bool check(bool for_white);
		void generateLegalMoves();
		void populateMoves();
		void initClassicGame();
		std::vector<MoveInfo> getBlackPseudoMoves();
		std::vector<MoveInfo> getWhitePseudoMoves();

		bool virtualMove(MoveInfo move);
		BoardState calculateState();
		OutCome gameLoop();
		BoardState move( bool white );
		bool move_turn();
		bool verifyPick(int r, int c);
		bool verifyMove(int r, int c, Piece* piece);

		// everything should end up private escpet the final startGame()


};