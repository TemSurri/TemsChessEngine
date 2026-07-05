#pragma once
#include <vector>
#include "piece.h"
#include <array>
#include <variant>
#include <unordered_map>
#include <cstdint>

// to do organize code to only need to use one array, and is turn thats all

class ClassicChess {

	enum OutCome {
		WhiteWin,
		BlackWin,
		Draw,
		Normal
	};



	enum MoveBunch :int {
		TT,
		CAPTURES,
		KM,
		QUIET
	};

	//multi purpose for move batches, TT, captures, killer moves, quiet moves. For orderingmoves in minimax
	// stores ptr to piece and all its moves
	struct MoveSet {
		
		
		std::vector<MoveEndpoint> moves;
		
	};

	struct EvaluatedMove {
		// value of minimax after it was searched
		int value;
		MoveEndpoint move;

	};

	struct MoveRecord {

		Piece* taken = nullptr;
		Piece* moved = nullptr;
		int startRow;
		int startCol;
		MoveEndpoint end;

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
		std::array<MoveSet, 4> legalMoves{};

		// actually store pieces
		std::vector<Piece> whitePieces;
		std::vector<Piece> blackPieces;

		//all board manipulation
		MoveRecord final_move(const MoveEndpoint& move);
		void undo_move(MoveRecord record);

		//setup
		Piece* storePiece(int r, int c, PieceType type);
		void initClassicGame();

		//move generation need to pass in turn for context
		bool is_checked(bool is_white);
		bool check(bool for_white);
		// checks if quadrant is attacked
		bool is_attacked(int r, int c, bool is_white);

		

		std::vector<MoveSet> getPseudoMoves(std::vector<Piece>& pieces);
	
		// piece aready passed in these 3 so no need to pass in turn
		void filterMoveSet(MoveSet& move, bool kingInCheck, Piece* piece);
		bool is_pinned(Piece& p);
		bool virtualMoveCauseCheck(MoveSet move);

		//Game Logic
		bool hasLegalMoves();
		OutCome calculateState();
		bool move_turn();
		bool verifyPick(int r, int c);
		std::variant<bool, MoveEndpoint> verifyMove(int r, int c, Piece* piece);

		// ai stuff

		enum FLAG {
			EXACT,
			UPPER_BOUND,
			LOWER_BOUND
		};

		struct TTEntry {
			MoveEndpoint move;
			int depth;
			int score;
			FLAG bound_type;
		};


		int evaluateBoard();
		MoveBunch analyzeMove(MoveEndpoint& move);
		EvaluatedMove getBestMove(int depth, bool maximizing);
		int minimax(int depth, bool maximizing, int alpha, int beta);
		const int whiteMaximizing = true;
		
		std::array<MoveSet, 4> GenerateOrderedLegalMoves(bool is_white);
	
		uint64_t getHashCode();
		std::unordered_map<uint64_t, TTEntry> transpositionalTable;
		// can generate hash itself : main function to add to TT
		void cacheEntryTT(const MoveEndpoint& move, int score, int depth, bool maximizing, bool pruned);

	public:

		ClassicChess() {
			
		};

		//move
		void printAllMoves();
		void printBoard();

		void gameLoop();
		void gameLoopVSminimaxAI(bool whiteIsAi, int depth);


		
		//minimax 
		
	
		
};