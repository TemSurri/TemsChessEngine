#include "game.h"
#include "vector"
#include <iostream>
#include <variant>

//PSUEDO LEGAL MOVE GENERATION
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

//LEGAL MOVE GENERATION
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

//calcaultes game state
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
	init_knight_attacks();
	init_king_attacks();
	init_bitboard();

	game = true;
	white_move = true;

	while (game)
	{
		std::cout << "VALUE OF BOARD: " << evaluateBoard() << '\n';

		print_bitboard();

		auto legal = generate_legal_moves(white_move);
		print_moves(legal);

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

	init_knight_attacks();
	init_king_attacks();
	init_bitboard();

	initZobrist();


	while (this->game) {

		std::cout << "VALUE OF BOARD:" << evaluateBoard() << std::endl;
		print_bitboard();


		uint64_t key = getHashCode(white_move);
		TTEntry& cached = transpositionalTable[key % TTsize];

		bool hasTT = (key == cached.id);

		Move orderTT{};
		if (hasTT) {
			orderTT = cached.move;
		}

		auto legalMoves = GenerateOrderedLegalMoves(white_move, orderTT, hasTT, depth);
		
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



