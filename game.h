#pragma once
#include <vector>
#include "piece.h"
#include <array>
#include <variant>
#include <unordered_map>
#include <cstdint>
#include <bit>
#include <random>

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

		//need to be replaced ----------------------------------------------
		Piece* board[BOARDROWS][BOARDCOLS]{nullptr};
		Piece* blackKing{ nullptr };
		Piece* whiteKing{ nullptr };

		// stores only moveable pieces and their respective moves
		std::array<MoveSet, 4> legalMoves{};

		//need to be replaced ----------------------------------------------
		std::vector<Piece> whitePieces;
		std::vector<Piece> blackPieces;

		//refactor 
		MoveRecord final_move(const MoveEndpoint& move);
		void undo_move(MoveRecord record);

		//need to be replaced ----------------------------------------------
		Piece* storePiece(int r, int c, PieceType type);
		void initClassicGame();

		//move generation need to pass in turn for context
		bool is_checked(bool is_white);
		bool check(bool for_white);
		// checks if quadrant is attacked
		bool is_attacked(int r, int c, bool is_white);

		//refactor 
		std::vector<MoveSet> getPseudoMoves(std::vector<Piece>& pieces);
	
		//refactor 
		void filterMoveSet(MoveSet& move, bool kingInCheck, Piece* piece);
		bool is_pinned(Piece& p);
		bool virtualMoveCauseCheck(MoveSet move);

		//Game Logic
		//refactor  maybe
		bool hasLegalMoves();
		OutCome calculateState();
		bool move_turn();
		bool verifyPick(int r, int c);
		std::variant<bool, MoveEndpoint> verifyMove(int r, int c, Piece* piece);

		// MINIMAX AI STUFF
		// TT caching
		//refactor
		uint64_t zobristID[64][2][6];
		uint64_t white_move_key;
		uint64_t random_u64() {
			static std::random_device rd;
			static std::mt19937_64 rng(rd());
			static std::uniform_int_distribution<uint64_t> dist;

			return dist(rng);
		}
		void initZobrist() {
			white_move_key = random_u64();


			for (int spot{}; spot < 64; spot++) {

				for (int color{}; color < 2; color++) {
					for (int piece{}; piece < 6; piece++) {

						zobristID[spot][color][piece] = random_u64();


					}
				}
			}
		}

		bool sameMove(const MoveEndpoint& a,
			const MoveEndpoint& b) const
		{
			return a.p == b.p &&
				a.r == b.r &&
				a.c == b.c;
		}

		enum FLAG {
			EXACT,
			UPPER_BOUND,
			LOWER_BOUND
		};

		struct TTEntry {
			uint64_t id;
			bool whitemove;
			MoveEndpoint move;
			int depth;
			int score;
			FLAG bound_type;
		};

		// MOVE ORDERING
		//refactor
		int evaluateBoard();
		MoveBunch analyzeMove(MoveEndpoint& move, const MoveEndpoint& TTmove, bool isTT, int depth);
		EvaluatedMove searchRoot(int depth, bool whiteToMove);
		EvaluatedMove getBestMoveIterative(int maxDepth, bool whiteToMove);
		int minimax(int depth, bool maximizing, int alpha, int beta);
		const int whiteMaximizing = true;
		
		std::array<MoveSet, 4> GenerateOrderedLegalMoves(bool is_white, const MoveEndpoint& TTmove, bool isTT, int depth);
		
		static constexpr size_t TTsize = 1 << 20;
		uint64_t getHashCode(bool whitemove);
		std::vector< TTEntry> transpositionalTable;
		// can generate hash itself : main function to add to TT
		void cacheEntryTT(TTEntry entry);

		struct SearchStats {
			uint64_t nodes = 0;
			uint64_t leafNodes = 0;
			uint64_t alphaBetaCutoffs = 0;
			uint64_t ttHits = 0;
			uint64_t ttStores = 0;
			uint64_t killerHits = 0;

			double elapsedMs = 0.0;
		};

		SearchStats stats;

		void resetSearchStats();
		void printSearchStats(int depth);

		//Killer moevs (quiet(non capture) moves that show some strat)
		static constexpr int MAX_SEARCH_DEPTH = 10;

		std::array<std::array<MoveEndpoint, 2>, MAX_SEARCH_DEPTH> killerMoves{};
		std::array<std::array<bool, 2>, MAX_SEARCH_DEPTH> killerValid{};

		bool isCaptureMove(const MoveEndpoint& move) const;
		void storeKillerMove(const MoveEndpoint& move, int depth);
		void clearKillerMoves();
	public:

		ClassicChess() :transpositionalTable(TTsize) {

		};

		//move
		void printAllMoves();
		void printBoard();

		void gameLoop();
		void gameLoopVSminimaxAI(bool whiteIsAi, int depth);


		//curent bitboard implementation



		uint64_t occupancy;
		uint64_t empty;
		uint64_t w_occupancy;
		uint64_t b_occupancy;

		uint64_t w_bishops;
		uint64_t w_pawns;
		uint64_t w_king;
		uint64_t w_rooks;
		uint64_t w_knights;
		uint64_t w_queen;

		uint64_t b_bishops;
		uint64_t b_pawns;
		uint64_t b_king;
		uint64_t b_rooks;
		uint64_t b_knights;
		uint64_t b_queen;

		enum PieceTypeBit {
			NO_PIECE,
			W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
			B_PAWN, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING
		};

		struct Move {
			int from;
			int to;

			PieceTypeBit moved;
			PieceTypeBit captured = NO_PIECE;
		};

		static constexpr int NORTH = 8;
		static constexpr int SOUTH = -8;

		static constexpr int EAST = 1;
		static constexpr int WEST = -1;

		static constexpr int NORTH_EAST = 9;
		static constexpr int NORTH_WEST = 7;

		static constexpr int SOUTH_EAST = -7;
		static constexpr int SOUTH_WEST = -9;

		uint64_t& get_piece_board(PieceTypeBit piece)
		{
			switch (piece)
			{
			case W_PAWN:   return w_pawns;
			case W_KNIGHT: return w_knights;
			case W_BISHOP: return w_bishops;
			case W_ROOK:   return w_rooks;
			case W_QUEEN:  return w_queen;
			case W_KING:   return w_king;

			case B_PAWN:   return b_pawns;
			case B_KNIGHT: return b_knights;
			case B_BISHOP: return b_bishops;
			case B_ROOK:   return b_rooks;
			case B_QUEEN:  return b_queen;
			case B_KING:   return b_king;

			default:
				throw std::runtime_error("Invalid piece type");
			}
		}

		void exec_move(Move move)
		{
			uint64_t fromMask = 1ULL << move.from;
			uint64_t toMask = 1ULL << move.to;

			// remove captured piece if there is one
			if (move.captured != NO_PIECE)
			{
				get_piece_board(move.captured) &= ~toMask;
			}

			// move piece
			uint64_t& board = get_piece_board(move.moved);

			board &= ~fromMask; // remove from old square
			board |= toMask;    // place on new square

			updateOccupancy();
		}

		void undo_move(Move move)
		{
			uint64_t fromMask = 1ULL << move.from;
			uint64_t toMask = 1ULL << move.to;

			// move piece back
			uint64_t& board = get_piece_board(move.moved);

			board &= ~toMask;
			board |= fromMask;

			// restore captured piece
			if (move.captured != NO_PIECE)
			{
				get_piece_board(move.captured) |= toMask;
			}

			updateOccupancy();
		}

		void updateOccupancy()
		{
			w_occupancy =
				w_pawns |
				w_knights |
				w_bishops |
				w_rooks |
				w_queen |
				w_king;

			b_occupancy =
				b_pawns |
				b_knights |
				b_bishops |
				b_rooks |
				b_queen |
				b_king;

			occupancy = w_occupancy | b_occupancy;
			empty = ~occupancy;
		}

		void init_bitboard() {
			// clear boards


			w_pawns = 0;
			w_knights = 0;
			w_bishops = 0;
			w_rooks = 0;
			w_queen = 0;
			w_king = 0;

			b_pawns = 0;
			b_knights = 0;
			b_bishops = 0;
			b_rooks = 0;
			b_queen = 0;
			b_king = 0;


			// White pawns
			for (int i = 0; i < 8; i++) {
				w_pawns |= 1ULL << (8 + i);
			}

			// Black pawns
			for (int i = 0; i < 8; i++) {
				b_pawns |= 1ULL << (48 + i);
			}

			// White pieces
			w_rooks |= (1ULL << 0) | (1ULL << 7);
			w_knights |= (1ULL << 1) | (1ULL << 6);
			w_bishops |= (1ULL << 2) | (1ULL << 5);
			w_queen |= (1ULL << 3);
			w_king |= (1ULL << 4);

			// Black pieces
			b_rooks |= (1ULL << 56) | (1ULL << 63);
			b_knights |= (1ULL << 57) | (1ULL << 62);
			b_bishops |= (1ULL << 58) | (1ULL << 61);
			b_queen |= (1ULL << 59);
			b_king |= (1ULL << 60);



			updateOccupancy();

		}
		
		void print_bitboard()
		{
			std::cout << "\n  A B C D E F G H\n";

			for (int rank = 7; rank >= 0; rank--)
			{
				std::cout << rank + 1 << " ";

				for (int file = 0; file < 8; file++)
				{
					int square = rank * 8 + file;
					uint64_t mask = 1ULL << square;

					char piece = '.';

					// White
					if (w_pawns & mask)      piece = 'P';
					else if (w_knights & mask) piece = 'N';
					else if (w_bishops & mask) piece = 'B';
					else if (w_rooks & mask)   piece = 'R';
					else if (w_queen & mask)   piece = 'Q';
					else if (w_king & mask)    piece = 'K';

					// Black
					else if (b_pawns & mask)   piece = 'p';
					else if (b_knights & mask) piece = 'n';
					else if (b_bishops & mask) piece = 'b';
					else if (b_rooks & mask)   piece = 'r';
					else if (b_queen & mask)   piece = 'q';
					else if (b_king & mask)    piece = 'k';

					std::cout << piece << ' ';
				}

				std::cout << rank + 1 << '\n';
			}

			std::cout << "  A B C D E F G H\n";
		}

		char piece_to_char(PieceTypeBit piece)
		{
			switch (piece)
			{
			case W_PAWN:
			case B_PAWN:   return 'P';

			case W_KNIGHT:
			case B_KNIGHT: return 'N';

			case W_BISHOP:
			case B_BISHOP: return 'B';

			case W_ROOK:
			case B_ROOK:   return 'R';

			case W_QUEEN:
			case B_QUEEN:  return 'Q';

			case W_KING:
			case B_KING:   return 'K';

			default:
				return '?';
			}
		}

		std::string square_to_string(int square)
		{
			char file = 'a' + (square % 8);
			char rank = '1' + (square / 8);

			return std::string{ file, rank };
		}

		void print_moves(const std::vector<Move>& moves)
		{
			std::cout << "\nGenerated " << moves.size() << " moves:\n\n";

			for (const Move& move : moves)
			{
				std::cout
					<< piece_to_char(move.moved)
					<< " "
					<< square_to_string(move.from)
					<< " -> "
					<< square_to_string(move.to);

				if (move.captured != NO_PIECE)
				{
					std::cout << "  captures "
						<< piece_to_char(move.captured);
				}

				std::cout << '\n';
			}
		}

		void test_in_main() {
			init_knight_attacks();
			init_king_attacks();
			init_bitboard();

			print_bitboard();


			auto moves = generate_pseudo_moves(true);
			print_moves(moves);
		}

		//MOVE GENERATION

		PieceTypeBit piece_on_square(int square)
		{
			uint64_t mask = 1ULL << square;

			if (w_pawns & mask) return W_PAWN;
			if (w_knights & mask) return W_KNIGHT;
			if (w_bishops & mask) return W_BISHOP;
			if (w_rooks & mask) return W_ROOK;
			if (w_queen & mask) return W_QUEEN;
			if (w_king & mask) return W_KING;

			if (b_pawns & mask) return B_PAWN;
			if (b_knights & mask) return B_KNIGHT;
			if (b_bishops & mask) return B_BISHOP;
			if (b_rooks & mask) return B_ROOK;
			if (b_queen & mask) return B_QUEEN;
			if (b_king & mask) return B_KING;

			return NO_PIECE;
		}

		void add_move(std::vector<Move>& moves, int from, int to, PieceTypeBit moved)
		{
			Move m;
			m.from = from;
			m.to = to;
			m.moved = moved;
			m.captured = piece_on_square(to);

			moves.push_back(m);
		}
		
		bool is_own_piece(int square, bool white)
		{
			uint64_t mask = 1ULL << square;
			return white ? (w_occupancy & mask) : (b_occupancy & mask);
		}

		bool is_enemy_piece(int square, bool white)
		{
			uint64_t mask = 1ULL << square;
			return white ? (b_occupancy & mask) : (w_occupancy & mask);
		}

		bool is_empty_square(int square)
		{
			uint64_t mask = 1ULL << square;
			return empty & mask;
		}


		std::vector<Move> generate_pseudo_moves(bool whiteToMove)
		{
			std::vector<Move> moves;
			moves.reserve(218); // reserve 218 thats the max # a baord state can have, this is just to avoid heap allocations

			updateOccupancy();

			if (whiteToMove)
			{
				generate_pawn_moves(moves, true);
				generate_knight_moves(moves, true);
				//generate_bishop_moves(moves, true);
				//generate_rook_moves(moves, true);
				//generate_queen_moves(moves, true);
				generate_king_moves(moves, true);
			}
			else
			{
				generate_pawn_moves(moves, false);
				generate_knight_moves(moves, false);
				//generate_bishop_moves(moves, false);
				//generate_rook_moves(moves, false);
				//generate_queen_moves(moves, false);
				generate_king_moves(moves, false);
			}

			return moves;
		}
		
		// removes the lowest right most bit. 
		inline int pop_lsb(uint64_t& bitboard)
		{
			int square = std::countr_zero(bitboard); //finds where the lowest right most bit is
			bitboard &= bitboard - 1; //pops it from the passed in board
			return square;
		}

		// is there a piece on this square on this bitboard?
		inline bool is_set(uint64_t bitboard, int square)
		{
			return (bitboard >> square) & 1ULL;
		}

		void generate_pawn_moves(std::vector<Move>& moves, bool is_white);

		void generate_knight_moves(std::vector<Move>& moves, bool is_white);
		uint64_t knight_attacks[64];
		// need to init at start of program
		void init_knight_attacks()
		{
			for (int square = 0; square < 64; square++)
			{
				uint64_t attacks = 0ULL;

				int rank = square / 8;
				int file = square % 8;

				const int dr[8] = { 2, 2, 1, 1, -1, -1, -2, -2 };
				const int df[8] = { 1, -1, 2, -2, 2, -2, 1, -1 };

				for (int i = 0; i < 8; i++)
				{
					int r = rank + dr[i];
					int f = file + df[i];

					if (r >= 0 && r < 8 && f >= 0 && f < 8)
					{
						int target = r * 8 + f;
						attacks |= 1ULL << target;
					}
				}

				knight_attacks[square] = attacks;
			}
		}

		//void generate_bishop_moves(std::vector<Move>& moves, bool is_white);
		//void generate_rook_moves(std::vector<Move>& moves, bool is_white);
		//void generate_queen_moves(std::vector<Move>& moves, bool is_white);
		void generate_king_moves(std::vector<Move>& moves, bool is_white);
		uint64_t king_attacks[64];
		// need to init at start of program
		void init_king_attacks()
		{
			for (int square = 0; square < 64; square++)
			{
				uint64_t attacks = 0ULL;

				int rank = square / 8;
				int file = square % 8;

				const int dr[8] = { 1, 1, 1, 0, 0, -1, -1, -1 };
				const int df[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };

				for (int i = 0; i < 8; i++)
				{
					int r = rank + dr[i];
					int f = file + df[i];

					if (r >= 0 && r < 8 && f >= 0 && f < 8)
					{
						attacks |= 1ULL << (r * 8 + f);
					}
				}

				king_attacks[square] = attacks;
			}
		}
		
};