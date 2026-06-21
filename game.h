#pragma once
#include <vector>
#include "piece.h"
#include <array>
#include <variant>

class ClassicChess {

	enum OutCome {
		WhiteWin,
		BlackWin,
		Draw,
		Normal
	};

	// stores ptr to piece and all its moves
	struct MoveSet {
		
		Piece* piece;
		std::vector<MoveEndpoint> moves;
		
	};

	private:
		bool game = true;
		bool white_upper = true;
		int iterator{};

		// to change every move
		bool white_move = true;

		Piece* board[BOARDROWS][BOARDCOLS]{nullptr};

		Piece* blackKing{ nullptr };
		Piece* whiteKing{ nullptr };

		// stores only moveable pieces and their respective moves
		std::vector<MoveSet> legalBlackMoves{};
		std::vector<MoveSet> legalWhiteMoves{};

		

	public:
		//public for now 

		std::vector<Piece> whitePieces;
		std::vector<Piece> blackPieces;
		//----------------

		ClassicChess() {
			
		};

		//move

		void final_move(Piece* p, MoveEndpoint move ) {
			const int ogR = p->getRow();
			const int ogC = p->getCol();
			const int newR = move.r;
			const int newC = move.c;

			if (move.fashion == CASTLE) {
				//do castle
			
				// add or subtract 2 fromt king col
				// rooks new col = king col + or minus 2

				// depends on if rook col > other col

				//idneitfy the rook
				Piece* rookToCastle = board[newR][newC];
				board[newR][newC] = nullptr;

				board[ogR][ogC] = nullptr;

				if (newC > ogC) {
					// rook is to the right of the king
					board[ogR][ogC + 2] = p;
					p->move(ogR, ogC + 2);
					p->incrementMove();

					board[ogR][ogC + 2 - 1] = rookToCastle;
					rookToCastle->move(ogR, ogC + 2 - 1);
					rookToCastle->incrementMove();

				}
				else {
					// rook is to the left of the king
					board[ogR][ogC - 2] = p;
					p->move(ogR, ogC - 2);
					p->incrementMove();

					board[ogR][ogC - 2 + 1] = rookToCastle;
					rookToCastle->move(ogR, ogC - 2 + 1);
					rookToCastle->incrementMove();

				}

				return;
			}


			if (board[newR][newC]) {
				board[newR][newC]->captured = true;
			}

			board[ogR][ogC]->incrementMove();
			board[ogR][ogC]->move(newR, newC);
			board[newR][newC] = board[ogR][ogC];

			board[ogR][ogC] = nullptr;

			if (move.fashion == PAWN_PROMOTION) {
				p->changeType(PieceType::Queen);
			}

			if (move.fashion == EN_PASSENT) {

				board[ogR][newC] = nullptr;

			};
			
			// auto empties old spot isnt accurate for castling
		};

		void undo_move(Piece* p, MoveEndpoint end, Piece* taken, std::array<int, 2> start) {
			const int newR = end.r;
			const int newC = end.c;

			if (end.fashion == EN_PASSENT) {

				board[end.r][start[1]] = taken;
				board[start[0]][start[1]] = p;

				p->move(start[0], start[1]);
				p->deincrementMove();	
				
			};

			if (end.fashion == CASTLE && taken) {
				//remove the caslte things
				board[end.r][end.c] = taken;
				board[start[0]][start[1]] = p;
				p->move(start[0], start[1]);
				p->deincrementMove();

				// in this case the taken is the rook that moved
				taken->move(end.r, end.c);
				taken->deincrementMove();

				// clear the moved spots
				if (newC > start[1]) {
					// rook is to the right of the king
					board[start[0]][start[1] + 2] = nullptr;

					board[start[0]][start[1] + 2 - 1] = nullptr;

				}
				else {
					// rook is to the right of the king
					board[start[0]][start[1] - 2] = nullptr;


					board[start[0]][start[1] - 2 + 1] = nullptr;

				}

				return;
			}

			if (end.fashion == PAWN_PROMOTION) {

				if (taken) {
					taken->captured = false;
				}

				p->changeType(Pawn);
				board[end.r][end.c] = taken;
				board[start[0]][start[1]] = p;
				p->move(start[0], start[1]);
				p->deincrementMove();

				return;

			}

			// might laso just encompass undo castle logic
			if (end.fashion == STANDARD) {

				if (taken) {
					taken->captured = false;
				}

				board[end.r][end.c] = taken;
				board[start[0]][start[1]] = p;
				p->move(start[0], start[1]);
				p->deincrementMove();

				return;
			}

		}
		
		//debug / helpers
		void printMoves(vector<array<int, 2>> list) {

			for (int i{}; i < list.size(); i++) {
				cout << "(" << list[i][0] << ", " << list[i][1] << ") " << endl;
			}
		}
		void printAllMoves();
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



			return;
		};


		//setup
		Piece* storePiece(int r, int c, PieceType type);
		void initClassicGame();


		//move generation
		bool is_checked(bool is_white);
		bool check(bool for_white);
		void generateLegalMoves();

		std::vector<MoveSet> getBlackPseudoMoves();
		std::vector<MoveSet> getWhitePseudoMoves();
	
		void filterMoveSet(MoveSet& move, bool kingInCheck);

		bool is_pinned(Piece& p);
		bool virtualMoveCauseCheck(MoveSet move);

		bool is_attacked(int r, int c, bool is_white);

		//Game Logic
		OutCome calculateState() {
			//
			if (white_move) {
				if (legalWhiteMoves.size() == 0) {
					if (is_checked(true)) {
						return BlackWin;
					}
					return Draw;
				}
			}
			else {
				if (legalBlackMoves.size() == 0) {
					if (is_checked(false)) {
						return WhiteWin;
					}
					return Draw;
				}
			}

			return Normal;

		};
		void gameLoop();
		bool move_turn();
		bool verifyPick(int r, int c);
		std::variant<bool, MoveEndpoint> verifyMove(int r, int c, Piece* piece);

		// everything should end up private escpet the final startGame()


};