#pragma once
#include <vector>
#include <array>
#include <variant>
#include <unordered_map>
#include <cstdint>
#include <bit>
#include <random>
#include <iostream>

// to do organize code to only need to use one array, and is turn thats all

class ClassicChess {

	enum PieceTypeBit {
		NO_PIECE,
		W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
		B_PAWN, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING
	};

	enum MoveType {
		NORMAL_MOVE,
		PROMOTION_MOVE,
		EN_PASSANT_MOVE,
		CASTLING_MOVE
	};

	struct Move {
		int from;
		int to;

		PieceTypeBit moved;
		PieceTypeBit captured = NO_PIECE;

		MoveType type = NORMAL_MOVE;

		int value;
	};

	struct MoveRecord {
		Move move;

		int oldEnPassantSquare;

		bool oldWhiteCastleKingSide;
		bool oldWhiteCastleQueenSide;
		bool oldBlackCastleKingSide;
		bool oldBlackCastleQueenSide;
	};

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
	struct MoveSet {
		
		
		std::vector<Move> moves;
		
	};

	struct EvaluatedMove {
		// value of minimax after it was searched
		int value;
		Move move;

	};

	private:
		// init 
		bool game = true;
		bool white_upper = true;

		// to change every move
		bool white_move = true;
		int iterator{};

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

		int en_passant_square = -1;

		bool white_can_castle_kingside = true;
		bool white_can_castle_queenside = true;
		bool black_can_castle_kingside = true;
		bool black_can_castle_queenside = true;

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

		MoveRecord exec_move(const Move& move)
		{
			MoveRecord record;
			record.move = move;

			record.oldEnPassantSquare = en_passant_square;
			record.oldWhiteCastleKingSide = white_can_castle_kingside;
			record.oldWhiteCastleQueenSide = white_can_castle_queenside;
			record.oldBlackCastleKingSide = black_can_castle_kingside;
			record.oldBlackCastleQueenSide = black_can_castle_queenside;

			uint64_t fromMask = 1ULL << move.from;
			uint64_t toMask = 1ULL << move.to;

			en_passant_square = -1;

			if (move.type == EN_PASSANT_MOVE)
			{
				uint64_t& movingBoard = get_piece_board(move.moved);

				movingBoard &= ~fromMask;
				movingBoard |= toMask;

				int capturedSquare = (move.moved == W_PAWN) ? move.to - 8 : move.to + 8;
				get_piece_board(move.captured) &= ~(1ULL << capturedSquare);
			}
			else if (move.type == PROMOTION_MOVE)
			{
				get_piece_board(move.moved) &= ~fromMask;

				if (move.captured != NO_PIECE)
					get_piece_board(move.captured) &= ~toMask;

				PieceTypeBit promoted = (move.moved == W_PAWN) ? W_QUEEN : B_QUEEN;
				get_piece_board(promoted) |= toMask;
			}
			else if (move.type == CASTLING_MOVE)
			{
				uint64_t& kingBoard = get_piece_board(move.moved);

				kingBoard &= ~fromMask;
				kingBoard |= toMask;

				if (move.to == 6) {
					w_rooks &= ~(1ULL << 7);
					w_rooks |= 1ULL << 5;
				}
				else if (move.to == 2) {
					w_rooks &= ~(1ULL << 0);
					w_rooks |= 1ULL << 3;
				}
				else if (move.to == 62) {
					b_rooks &= ~(1ULL << 63);
					b_rooks |= 1ULL << 61;
				}
				else if (move.to == 58) {
					b_rooks &= ~(1ULL << 56);
					b_rooks |= 1ULL << 59;
				}
			}
			else
			{
				if (move.captured != NO_PIECE)
					get_piece_board(move.captured) &= ~toMask;

				uint64_t& movingBoard = get_piece_board(move.moved);

				movingBoard &= ~fromMask;
				movingBoard |= toMask;

				if (move.moved == W_PAWN && move.to - move.from == 16)
					en_passant_square = move.from + 8;

				if (move.moved == B_PAWN && move.from - move.to == 16)
					en_passant_square = move.from - 8;
			}

			// update castling rights
			if (move.moved == W_KING) {
				white_can_castle_kingside = false;
				white_can_castle_queenside = false;
			}
			else if (move.moved == B_KING) {
				black_can_castle_kingside = false;
				black_can_castle_queenside = false;
			}

			if (move.moved == W_ROOK && move.from == 0) white_can_castle_queenside = false;
			if (move.moved == W_ROOK && move.from == 7) white_can_castle_kingside = false;
			if (move.moved == B_ROOK && move.from == 56) black_can_castle_queenside = false;
			if (move.moved == B_ROOK && move.from == 63) black_can_castle_kingside = false;

			if (move.captured == W_ROOK && move.to == 0) white_can_castle_queenside = false;
			if (move.captured == W_ROOK && move.to == 7) white_can_castle_kingside = false;
			if (move.captured == B_ROOK && move.to == 56) black_can_castle_queenside = false;
			if (move.captured == B_ROOK && move.to == 63) black_can_castle_kingside = false;

			updateOccupancy();
			return record;
		}

		void undo_move(const MoveRecord& record)
		{
			const Move& move = record.move;

			uint64_t fromMask = 1ULL << move.from;
			uint64_t toMask = 1ULL << move.to;

			if (move.type == EN_PASSANT_MOVE)
			{
				uint64_t& movingBoard = get_piece_board(move.moved);

				movingBoard &= ~toMask;
				movingBoard |= fromMask;

				int capturedSquare = (move.moved == W_PAWN) ? move.to - 8 : move.to + 8;
				get_piece_board(move.captured) |= 1ULL << capturedSquare;
			}
			else if (move.type == PROMOTION_MOVE)
			{
				PieceTypeBit promoted = (move.moved == W_PAWN) ? W_QUEEN : B_QUEEN;

				get_piece_board(promoted) &= ~toMask;
				get_piece_board(move.moved) |= fromMask;

				if (move.captured != NO_PIECE)
					get_piece_board(move.captured) |= toMask;
			}
			else if (move.type == CASTLING_MOVE)
			{
				uint64_t& kingBoard = get_piece_board(move.moved);

				kingBoard &= ~toMask;
				kingBoard |= fromMask;

				if (move.to == 6) {
					w_rooks &= ~(1ULL << 5);
					w_rooks |= 1ULL << 7;
				}
				else if (move.to == 2) {
					w_rooks &= ~(1ULL << 3);
					w_rooks |= 1ULL << 0;
				}
				else if (move.to == 62) {
					b_rooks &= ~(1ULL << 61);
					b_rooks |= 1ULL << 63;
				}
				else if (move.to == 58) {
					b_rooks &= ~(1ULL << 59);
					b_rooks |= 1ULL << 56;
				}
			}
			else
			{
				uint64_t& movingBoard = get_piece_board(move.moved);

				movingBoard &= ~toMask;
				movingBoard |= fromMask;

				if (move.captured != NO_PIECE)
					get_piece_board(move.captured) |= toMask;
			}

			en_passant_square = record.oldEnPassantSquare;

			white_can_castle_kingside = record.oldWhiteCastleKingSide;
			white_can_castle_queenside = record.oldWhiteCastleQueenSide;
			black_can_castle_kingside = record.oldBlackCastleKingSide;
			black_can_castle_queenside = record.oldBlackCastleQueenSide;

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
			case W_PAWN:   return 'P';
			case W_KNIGHT: return 'N';
			case W_BISHOP: return 'B';
			case W_ROOK:   return 'R';
			case W_QUEEN:  return 'Q';
			case W_KING:   return 'K';

			case B_PAWN:   return 'p';
			case B_KNIGHT: return 'n';
			case B_BISHOP: return 'b';
			case B_ROOK:   return 'r';
			case B_QUEEN:  return 'q';
			case B_KING:   return 'k';

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

		int inline row_col_to_square(int row, int col);

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
				generate_bishop_moves(moves, true);
				generate_rook_moves(moves, true);
				generate_queen_moves(moves, true);
				generate_king_moves(moves, true);
			}
			else
			{
				generate_pawn_moves(moves, false);
				generate_knight_moves(moves, false);
				generate_bishop_moves(moves, false);
				generate_rook_moves(moves, false);
				generate_queen_moves(moves, false);
				generate_king_moves(moves, false);
			}

			return moves;
		}

		// MOVE GENERATION HELPERS VERY IMPORTANT
		//-------------------------------------
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
		//-------------------------------------


		//pawn logic
		void add_pawn_move(std::vector<Move>& moves, int from, int to, PieceTypeBit pawnType)
		{
			Move m;
			m.from = from;
			m.to = to;
			m.moved = pawnType;
			m.captured = piece_on_square(to);

			if ((pawnType == W_PAWN && to >= 56) ||
				(pawnType == B_PAWN && to <= 7))
			{
				m.type = PROMOTION_MOVE; // auto queen
			}

			moves.push_back(m);
		}
		void generate_pawn_moves(std::vector<Move>& moves, bool is_white);

		//precomputed pieces logic

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
		void add_castling_moves(std::vector<Move>& moves, bool is_white);

		//sliding pieces logic
		inline bool is_valid_slide(int from, int to, int dir)
		{
			if (to < 0 || to >= 64)
				return false;

			int fromFile = from % 8;
			int toFile = to % 8;

			// moving east means file must increase by 1
			if (dir == EAST || dir == NORTH_EAST || dir == SOUTH_EAST)
				return toFile == fromFile + 1;

			// moving west means file must decrease by 1
			if (dir == WEST || dir == NORTH_WEST || dir == SOUTH_WEST)
				return toFile == fromFile - 1;

			// north/south file stays the same
			return toFile == fromFile;
		}
		void generate_sliding_moves(
			std::vector<Move>& moves,
			uint64_t pieces,
			bool is_white,
			PieceTypeBit pieceType,
			const int* dirs,
			int dirCount
		)
		{
			uint64_t own = is_white ? w_occupancy : b_occupancy;
			uint64_t enemy = is_white ? b_occupancy : w_occupancy;

			while (pieces)
			{
				int from = pop_lsb(pieces);

				for (int i = 0; i < dirCount; i++)
				{
					int dir = dirs[i];
					int current = from;

					while (true)
					{
						int to = current + dir;

						if (!is_valid_slide(current, to, dir))
							break;

						uint64_t toMask = 1ULL << to;

						if (own & toMask)
							break;

						add_move(moves, from, to, pieceType);

						if (enemy & toMask)
							break;

						current = to;
					}
				}
			}
		}

		void generate_bishop_moves(std::vector<Move>& moves, bool is_white);
		void generate_rook_moves(std::vector<Move>& moves, bool is_white);
		void generate_queen_moves(std::vector<Move>& moves, bool is_white);

		//MOVE FILTERING (LEGAL MOVE GENERATION)

		int get_king_square(bool white)
		{
			uint64_t king = white ? w_king : b_king;

			if (!king)
				return -1;

			return std::countr_zero(king);
		}

		bool is_king_in_check(bool whiteKing)
		{
			int kingSquare = get_king_square(whiteKing);

			if (kingSquare == -1)
				return false;

			return is_square_attacked(kingSquare, !whiteKing);
		}

		bool is_square_attacked_by_sliders(int square, bool byWhite);
		bool is_square_attacked(int square, bool byWhite);

		std::vector<Move> generate_legal_moves(bool whiteToMove)
		{
			std::vector<Move> pseudoMoves = generate_pseudo_moves(whiteToMove);

			std::vector<Move> legalMoves;
			legalMoves.reserve(pseudoMoves.size());

			bool currentlyInCheck = is_king_in_check(whiteToMove);

			for (const Move& move : pseudoMoves)
			{
				if (!currentlyInCheck && !can_be_pinned_to_king(move.from, whiteToMove))
				{
					legalMoves.push_back(move);
					continue;
				}

				auto record = exec_move(move);

				if (!is_king_in_check(whiteToMove))
				{
					legalMoves.push_back(move);
				}

				undo_move(record);
			}

			return legalMoves;
		}

		bool can_be_pinned_to_king(int from, bool white)
		{
			int kingSquare = get_king_square(white);

			if (kingSquare == -1)
				return true;

			int fromRank = from / 8;
			int fromFile = from % 8;

			int kingRank = kingSquare / 8;
			int kingFile = kingSquare % 8;

			// same file
			if (fromFile == kingFile)
				return true;

			// same rank
			if (fromRank == kingRank)
				return true;

			// same diagonal
			if (std::abs(fromRank - kingRank) == std::abs(fromFile - kingFile))
				return true;

			return false;
		};
		
		// MINIMAX AI STUFF
		// TT caching
	
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

		bool sameMove(const Move& a,
			const Move& b) const
		{
			return	a.to == b.to &&
					a.from == b.from;
		}

		enum FLAG {
			EXACT,
			UPPER_BOUND,
			LOWER_BOUND
		};

		struct TTEntry {
			uint64_t id;
			bool whitemove;
			Move move;
			int depth;
			int score;
			FLAG bound_type;
		};

		// MOVE ORDERING

		int piece_value(PieceTypeBit piece) const;
		int evaluateBoard();
		MoveBunch analyzeMove(Move& move, const Move& TTmove, bool isTT, int depth);
		EvaluatedMove searchRoot(int depth, bool whiteToMove, const std::array<ClassicChess::MoveSet, 4>& legalMoves);
		EvaluatedMove getBestMoveIterative(int maxDepth, bool whiteToMove);
		int minimax(int depth, bool maximizing, int alpha, int beta);
		const int whiteMaximizing = true;
		
		std::array<MoveSet, 4> GenerateOrderedLegalMoves(bool is_white, const Move& TTmove, bool isTT, int depth);
		
		static constexpr size_t TTsize = 1 << 20;
		void hash_piece_board(
			uint64_t pieces,
			int color,
			int pieceIndex,
			uint64_t& hash
		);
		uint64_t getHashCode(bool whitemove);
		std::vector< TTEntry> transpositionalTable;
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

		static constexpr int MAX_SEARCH_DEPTH = 10;

		std::array<std::array<Move, 2>, MAX_SEARCH_DEPTH> killerMoves{};
		std::array<std::array<bool, 2>, MAX_SEARCH_DEPTH> killerValid{};

		bool isCaptureMove(const Move& move) const;
		void storeKillerMove(const Move& move, int depth);
		void clearKillerMoves();
	public:

		ClassicChess() :transpositionalTable(TTsize) {

		};

		//move
		void printAllMoves();
		void printBoard();

		void gameLoop();
		void gameLoopVSminimaxAI(bool whiteIsAi, int depth);
		bool move_turn();
		std::variant<bool, Move> verifyMove(int from, int to);
		bool verifyPick(int r, int c);
		ClassicChess::OutCome calculateState();
};