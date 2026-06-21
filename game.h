#pragma once
#include <vector>
#include "piece.h"
#include <array>
#include <variant>

// to do organize code to only need to use one array, and is turn thats all



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
		// init 
		bool game = true;
		bool white_upper = true;

		// to change every move
		bool white_move = true;
		int iterator{};

		Piece* board[BOARDROWS][BOARDCOLS]{nullptr};

		Piece* blackKing{ nullptr };
		Piece* whiteKing{ nullptr };

		// stores only moveable pieces and their respective moves
		std::vector<MoveSet> legalBlackMoves{};
		std::vector<MoveSet> legalWhiteMoves{};

		// actually store pieces
		std::vector<Piece> whitePieces;
		std::vector<Piece> blackPieces;

		//all board manipulation
		void final_move(Piece* p, MoveEndpoint move);
		void undo_move(Piece* p, MoveEndpoint end, Piece* taken, std::array<int, 2> start);

		//setup
		Piece* storePiece(int r, int c, PieceType type);
		void initClassicGame();

		//move generation need to pass in turn for context
		bool is_checked(bool is_white);
		bool check(bool for_white);
		// checks if quadrant is attacked
		bool is_attacked(int r, int c, bool is_white);
		void generateLegalMoves(); // have to refator this to pass in turn

		std::vector<MoveSet> getBlackPseudoMoves();
		std::vector<MoveSet> getWhitePseudoMoves();
	
		// piece aready passed in these 3 so no need to pass in turn
		void filterMoveSet(MoveSet& move, bool kingInCheck);
		bool is_pinned(Piece& p);
		bool virtualMoveCauseCheck(MoveSet move);

		

		//Game Logic
		OutCome calculateState();
		bool move_turn();
		bool verifyPick(int r, int c);
		std::variant<bool, MoveEndpoint> verifyMove(int r, int c, Piece* piece);


	public:

		ClassicChess() {
			
		};

		//move
		void printAllMoves();
		void printBoard();

		void gameLoop();



};