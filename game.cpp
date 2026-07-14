#include "game.h"

#include <iostream>
#include <variant>
#include <vector>

// =============================================================================
// ClassicChess implementation
//
// This file contains board queries, move generation, legal-move filtering,
// state changes, console game loops, and the small public API used by the GUI.
// .
// =============================================================================

//MOVE GENERATION HELPERS -------------------------------------------

ClassicChess::PieceTypeBit ClassicChess::piece_on_square(int square) const
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

bool ClassicChess::is_own_piece(int square, bool white)
{
	uint64_t mask = 1ULL << square;
	return white ? (w_occupancy & mask) : (b_occupancy & mask);
}

bool ClassicChess::is_enemy_piece(int square, bool white)
{
	uint64_t mask = 1ULL << square;
	return white ? (b_occupancy & mask) : (w_occupancy & mask);
}

bool ClassicChess::is_empty_square(int square)
{
	uint64_t mask = 1ULL << square;
	return empty & mask;
}

//sliding pieces logic
inline bool ClassicChess::is_valid_slide(int from, int to, int dir)
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

void ClassicChess::generate_sliding_moves(std::vector<Move>& moves, uint64_t pieces, bool is_white, PieceTypeBit pieceType, const int* dirs, int dirCount)
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

void ClassicChess::add_move(std::vector<Move>& moves, int from, int to, PieceTypeBit moved)
{
	Move m;
	m.from = from;
	m.to = to;
	m.moved = moved;
	m.captured = piece_on_square(to);

	moves.push_back(m);
}

void ClassicChess::add_pawn_move(std::vector<Move>& moves, int from, int to, PieceTypeBit pawnType)
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

void ClassicChess::init_knight_attacks()
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

void ClassicChess::init_king_attacks()
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

//PSUEDO LEGAL MOVE GENERATION ------------------------------------------------------------

void ClassicChess::generate_pawn_moves(std::vector<Move>& moves, bool white)
{
	uint64_t pawns = white ? w_pawns : b_pawns;

	const PieceTypeBit pawnType = white ? W_PAWN : B_PAWN;
	const int forward = white ? NORTH : SOUTH;

	const int startMin = white ? 8 : 48;
	const int startMax = white ? 15 : 55;

	const uint64_t enemy = white ? b_occupancy : w_occupancy;

	while (pawns)
	{
		int from = pop_lsb(pawns);

		int oneForward = from + forward;

		// one-square push
		if (oneForward >= 0 && oneForward < 64 && is_set(empty, oneForward))
		{
			add_pawn_move(moves, from, oneForward, pawnType);

			// two-square push
			int twoForward = from + 2 * forward;

			if (from >= startMin && from <= startMax && is_set(empty, twoForward))
			{
				add_move(moves, from, twoForward, pawnType);
			}
		}

		// normal captures
		int capLeft = white ? from + NORTH_WEST : from + SOUTH_WEST;
		int capRight = white ? from + NORTH_EAST : from + SOUTH_EAST;

		if ((from % 8 != 0) && capLeft >= 0 && capLeft < 64 && is_set(enemy, capLeft))
		{
			add_pawn_move(moves, from, capLeft, pawnType);
		}

		if ((from % 8 != 7) && capRight >= 0 && capRight < 64 && is_set(enemy, capRight))
		{
			add_pawn_move(moves, from, capRight, pawnType);
		}

		// en passant
		if (en_passant_square != -1)
		{
			if ((from % 8 != 0) && capLeft == en_passant_square)
			{
				Move m;
				m.from = from;
				m.to = capLeft;
				m.moved = pawnType;
				m.captured = white ? B_PAWN : W_PAWN;
				m.type = EN_PASSANT_MOVE;

				moves.push_back(m);
			}

			if ((from % 8 != 7) && capRight == en_passant_square)
			{
				Move m;
				m.from = from;
				m.to = capRight;
				m.moved = pawnType;
				m.captured = white ? B_PAWN : W_PAWN;
				m.type = EN_PASSANT_MOVE;

				moves.push_back(m);
			}
		}
	}
}

void ClassicChess::generate_knight_moves(std::vector<Move>& moves, bool white)
{
	uint64_t knights = white ? w_knights : b_knights;
	uint64_t own = white ? w_occupancy : b_occupancy;

	PieceTypeBit knightType = white ? W_KNIGHT : B_KNIGHT;

	while (knights)
	{
		int from = pop_lsb(knights);

		uint64_t targets = knight_attacks[from] & ~own; //removes all the targets that overlap with your own occupancy

		while (targets)
		{
			int to = pop_lsb(targets);
			add_move(moves, from, to, knightType);
		}
	}
}

void ClassicChess::generate_king_moves(std::vector<Move>& moves, bool white)
{
	uint64_t king = white ? w_king : b_king;
	uint64_t own = white ? w_occupancy : b_occupancy;

	PieceTypeBit kingType = white ? W_KING : B_KING;

	if (!king) return;

	int from = pop_lsb(king);

	uint64_t targets = king_attacks[from] & ~own;

	while (targets)
	{
		int to = pop_lsb(targets);
		add_move(moves, from, to, kingType);
	}

	add_castling_moves(moves, white);
}

void ClassicChess::add_castling_moves(std::vector<Move>& moves, bool is_white)
{
	updateOccupancy();

	if (is_king_in_check(is_white))
		return;

	if (is_white)
	{

		if (white_can_castle_kingside &&
			(w_king & (1ULL << 4)) &&
			(w_rooks & (1ULL << 7)) &&
			!(occupancy & (1ULL << 5)) &&
			!(occupancy & (1ULL << 6)) &&
			!is_square_attacked(5, false) &&
			!is_square_attacked(6, false))
		{
			Move m;
			m.from = 4;
			m.to = 6;
			m.moved = W_KING;
			m.type = CASTLING_MOVE;
			moves.push_back(m);
		}

		if (white_can_castle_queenside &&
			(w_king & (1ULL << 4)) &&
			(w_rooks & (1ULL << 0)) &&
			!(occupancy & (1ULL << 1)) &&
			!(occupancy & (1ULL << 2)) &&
			!(occupancy & (1ULL << 3)) &&
			!is_square_attacked(3, false) &&
			!is_square_attacked(2, false))
		{
			Move m;
			m.from = 4;
			m.to = 2;
			m.moved = W_KING;
			m.type = CASTLING_MOVE;
			moves.push_back(m);
		}
	}
	else
	{
		if (black_can_castle_kingside &&
			(b_king & (1ULL << 60)) &&
			(b_rooks & (1ULL << 63)) &&
			!(occupancy & (1ULL << 61)) &&
			!(occupancy & (1ULL << 62)) &&
			!is_square_attacked(61, true) &&
			!is_square_attacked(62, true))
		{
			Move m;
			m.from = 60;
			m.to = 62;
			m.moved = B_KING;
			m.type = CASTLING_MOVE;
			moves.push_back(m);
		}

		if (black_can_castle_queenside &&
			(b_king & (1ULL << 60)) &&
			(b_rooks & (1ULL << 56)) &&
			!(occupancy & (1ULL << 57)) &&
			!(occupancy & (1ULL << 58)) &&
			!(occupancy & (1ULL << 59)) &&
			!is_square_attacked(59, true) &&
			!is_square_attacked(58, true))
		{
			Move m;
			m.from = 60;
			m.to = 58;
			m.moved = B_KING;
			m.type = CASTLING_MOVE;
			moves.push_back(m);
		}
	}
}

void ClassicChess::generate_bishop_moves(std::vector<Move>& moves, bool is_white)
{
	static constexpr int dirs[4] = {
		NORTH_EAST,
		NORTH_WEST,
		SOUTH_EAST,
		SOUTH_WEST
	};

	generate_sliding_moves(
		moves,
		is_white ? w_bishops : b_bishops,
		is_white,
		is_white ? W_BISHOP : B_BISHOP,
		dirs,
		4
	);
}

void ClassicChess::generate_rook_moves(std::vector<Move>& moves, bool is_white)
{
	static constexpr int dirs[4] = {
		NORTH,
		SOUTH,
		EAST,
		WEST
	};

	generate_sliding_moves(
		moves,
		is_white ? w_rooks : b_rooks,
		is_white,
		is_white ? W_ROOK : B_ROOK,
		dirs,
		4
	);
}

void ClassicChess::generate_queen_moves(std::vector<Move>& moves, bool is_white)
{
	static constexpr int dirs[8] = {
		NORTH,
		SOUTH,
		EAST,
		WEST,
		NORTH_EAST,
		NORTH_WEST,
		SOUTH_EAST,
		SOUTH_WEST
	};

	generate_sliding_moves(
		moves,
		is_white ? w_queen : b_queen,
		is_white,
		is_white ? W_QUEEN : B_QUEEN,
		dirs,
		8
	);
}

std::vector<ClassicChess::Move> ClassicChess::generate_pseudo_moves(bool whiteToMove)
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

//LEGAL MOVE GENERATION -------------------------------------------------------
bool ClassicChess::is_square_attacked_by_sliders(int square, bool byWhite)
{
	static constexpr int bishopDirs[4] = {
		NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST
	};

	static constexpr int rookDirs[4] = {
		NORTH, SOUTH, EAST, WEST
	};

	for (int dir : bishopDirs)
	{
		int current = square;

		while (true)
		{
			int to = current + dir;

			if (!is_valid_slide(current, to, dir))
				break;

			uint64_t toMask = 1ULL << to;

			if (occupancy & toMask)
			{
				PieceTypeBit p = piece_on_square(to);

				if (byWhite && (p == W_BISHOP || p == W_QUEEN))
					return true;

				if (!byWhite && (p == B_BISHOP || p == B_QUEEN))
					return true;

				break;
			}

			current = to;
		}
	}

	for (int dir : rookDirs)
	{
		int current = square;

		while (true)
		{
			int to = current + dir;

			if (!is_valid_slide(current, to, dir))
				break;

			uint64_t toMask = 1ULL << to;

			if (occupancy & toMask)
			{
				PieceTypeBit p = piece_on_square(to);

				if (byWhite && (p == W_ROOK || p == W_QUEEN))
					return true;

				if (!byWhite && (p == B_ROOK || p == B_QUEEN))
					return true;

				break;
			}

			current = to;
		}
	}

	return false;
}

bool ClassicChess::is_square_attacked(int square, bool byWhite)
{
	updateOccupancy();

	uint64_t squareMask = 1ULL << square;

	// pawn attacks
	if (byWhite)
	{
		if (square % 8 != 0)
		{
			int pawnSquare = square + SOUTH_EAST; // square - 7
			if (pawnSquare >= 0 && pawnSquare < 64 && (w_pawns & (1ULL << pawnSquare)))
				return true;
		}

		if (square % 8 != 7)
		{
			int pawnSquare = square + SOUTH_WEST; // square - 9
			if (pawnSquare >= 0 && pawnSquare < 64 && (w_pawns & (1ULL << pawnSquare)))
				return true;
		}
	}
	else
	{
		if (square % 8 != 0)
		{
			int pawnSquare = square + NORTH_EAST; // square + 9
			if (pawnSquare >= 0 && pawnSquare < 64 && (b_pawns & (1ULL << pawnSquare)))
				return true;
		}

		if (square % 8 != 7)
		{
			int pawnSquare = square + NORTH_WEST; // square + 7
			if (pawnSquare >= 0 && pawnSquare < 64 && (b_pawns & (1ULL << pawnSquare)))
				return true;
		}
	}

	// knight attacks
	uint64_t attackingKnights = byWhite ? w_knights : b_knights;

	if (knight_attacks[square] & attackingKnights)
		return true;

	// king attacks
	uint64_t attackingKing = byWhite ? w_king : b_king;

	if (king_attacks[square] & attackingKing)
		return true;

	// bishop / rook / queen attacks
	if (is_square_attacked_by_sliders(square, byWhite))
		return true;

	return false;
}

std::vector<ClassicChess::Move> ClassicChess::generate_legal_moves(bool whiteToMove)
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

bool ClassicChess::can_be_pinned_to_king(int from, bool white)
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


//STATE CHANGE -------------------------------------------------------
ClassicChess::MoveRecord ClassicChess::exec_move(const Move& move)
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

void ClassicChess::undo_move(const MoveRecord& record)
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

void ClassicChess::updateOccupancy()
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

void ClassicChess::init_bitboard() {
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

void ClassicChess::print_bitboard()
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



//GAME LOOP  ------------------------------------------------------------------------
ClassicChess::OutCome ClassicChess::calculateState()
{
	auto moves = generate_legal_moves(white_move);

	if (!moves.empty())
		return Normal;

	if (is_king_in_check(white_move))
	{
		return white_move ? BlackWin : WhiteWin;
	}

	return Draw;
}

char ClassicChess::piece_to_char(PieceTypeBit piece)
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

std::string ClassicChess::square_to_string(int square)
{
	char file = 'a' + (square % 8);
	char rank = '1' + (square / 8);

	return std::string{ file, rank };
}

void ClassicChess::print_moves(const std::vector<Move>& moves)
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

int inline ClassicChess::row_col_to_square(int row, int col)
{
	return row * 8 + col;
}

//validate user input to pick
bool ClassicChess::verifyPick(int r, int c)
{
	if (r < 0 || r >= 8 || c < 0 || c >= 8)
		return false;

	int square = row_col_to_square(r, c);
	uint64_t mask = 1ULL << square;

	if (white_move)
		return w_occupancy & mask;
	else
		return b_occupancy & mask;
}

//validate user input to pick to move
std::variant<bool, ClassicChess::Move> ClassicChess::verifyMove(int from, int to)
{
	auto moves = generate_legal_moves(white_move);

	for (const Move& move : moves)
	{
		if (move.from == from && move.to == to)
		{
			return move;
		}
	}

	return false;
}

//player turn
bool ClassicChess::move_turn()
{
	int req_row;
	int req_col;

	std::cout << "Pick Piece:\n";
	std::cout << (white_move ? "WHITE MOVE\n" : "BLACK MOVE\n");

	std::cout << "row: ";
	std::cin >> req_row;

	std::cout << "col: ";
	std::cin >> req_col;

	while (!verifyPick(req_row, req_col))
	{
		std::cout << "Please pick a valid piece:\n";

		std::cout << "row: ";
		std::cin >> req_row;

		std::cout << "col: ";
		std::cin >> req_col;
	}

	int from = row_col_to_square(req_row, req_col);

	int move_row;
	int move_col;

	std::cout << "Move Piece:\n";

	std::cout << "row: ";
	std::cin >> move_row;

	std::cout << "col: ";
	std::cin >> move_col;

	if (move_row < 0 || move_row >= 8 || move_col < 0 || move_col >= 8)
	{
		std::cout << "Invalid square.\n";
		return false;
	}

	int to = row_col_to_square(move_row, move_col);

	std::variant<bool, Move> moveResult = verifyMove(from, to);

	while (std::holds_alternative<bool>(moveResult))
	{
		std::cout << "Please pick a valid move:\n";

		std::cout << "row: ";
		std::cin >> move_row;

		std::cout << "col: ";
		std::cin >> move_col;

		if (move_row < 0 || move_row >= 8 || move_col < 0 || move_col >= 8)
			continue;

		to = row_col_to_square(move_row, move_col);

		moveResult = verifyMove(from, to);
	}

	Move chosenMove = std::get<Move>(moveResult);

	exec_move(chosenMove);

	return true;
}

//regular pvp gameloop
void ClassicChess::gameLoop()
{

	init_bitboard();

	game = true;
	white_move = true;

	while (game)
	{
		std::cout << "VALUE OF BOARD: " << evaluateBoard() << '\n';

		print_bitboard();

		auto outcome = calculateState();

		std::cout << "outcome: " << outcome << '\n';

		if (outcome != Normal)
		{
			if (outcome == BlackWin)
				std::cout << "Black wins by checkmate\n";
			else if (outcome == WhiteWin)
				std::cout << "White wins by checkmate\n";
			else
				std::cout << "Draw by stalemate\n";

			game = false;
			return;
		}

		bool turn = move_turn();

		if (turn)
		{
			iterator++;
			white_move = !white_move;
		}
	}
}

//ai vs player gameloop
void ClassicChess::gameLoopVSminimaxAI(bool whiteIsAi, int depth) {

	init_bitboard();

	while (this->game) {

		std::cout << "VALUE OF BOARD:" << evaluateBoard() << std::endl;
		print_bitboard();


		uint64_t key = getHashCode(white_move);
		TTEntry& cached = transpositionalTable[key & (TTsize - 1)];

		bool hasTT = (key == cached.id);

		Move orderTT{};
		if (hasTT) {
			orderTT = cached.move;
		}

		auto outCome = calculateState();
		std::cout << "outcome : " << outCome << std::endl;
		if (outCome != Normal) {
			if (outCome == BlackWin) {
				std::cout << "Black wins by checkmate\n";
			}
			else if (outCome == WhiteWin) {
				std::cout << "White wins by checkmate\n";
			}
			else if (outCome == Draw) {
				std::cout << "Draw by stalemate\n";
			}
			game = false;
			return;
		}

		//white always starts

		bool ai_move = (whiteIsAi == white_move);
		if (ai_move) {

			auto bestMove = getBestMoveIterative(depth, white_move);
			exec_move(bestMove.move);

			//ai move

		}
		else {

			bool turn = move_turn();

		}

		iterator += 1;
		if (white_move) {
			white_move = false;
		}
		else {
			white_move = true;
		}

	}
}

//gui access
void ClassicChess::resetGame()
{
	init_bitboard();
	updateOccupancy();

	white_move = true;
	game = true;
	iterator = 0;

	en_passant_square = -1;

	white_can_castle_kingside = true;
	white_can_castle_queenside = true;
	black_can_castle_kingside = true;
	black_can_castle_queenside = true;

	clearKillerMoves();

	for (TTEntry& entry : transpositionalTable)
	{
		entry = TTEntry{};
	}

	resetSearchStats();
}

bool ClassicChess::tryMove(int from, int to)
{
	if (from < 0 || from >= 64)
		return false;

	if (to < 0 || to >= 64)
		return false;

	// verifyMove already generates legal moves for the current side.
	std::variant<bool, Move> result = verifyMove(from, to);

	if (std::holds_alternative<bool>(result))
	{
		return false;
	}

	const Move move = std::get<Move>(result);

	exec_move(move);

	white_move = !white_move;
	iterator++;

	return true;
}

bool ClassicChess::isWhiteTurn() const
{
	return white_move;
}

ClassicChess::OutCome ClassicChess::getGameState()
{
	return calculateState();
}

bool ClassicChess::makeAIMove(int depth)
{
	const OutCome outcome = calculateState();

	if (outcome != Normal)
	{
		return false;
	}

	const EvaluatedMove bestMove =
		getBestMoveIterative(depth, white_move);

	// This protects against an invalid result if no move was found.
	if (bestMove.move.from < 0 ||
		bestMove.move.from >= 64 ||
		bestMove.move.to < 0 ||
		bestMove.move.to >= 64)
	{
		return false;
	}

	exec_move(bestMove.move);

	white_move = !white_move;
	iterator++;

	return true;
}