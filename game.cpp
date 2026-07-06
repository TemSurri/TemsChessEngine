#include "game.h"
#include "vector"
#include <iostream>
#include <variant>

//DEBUG----------------------
void ClassicChess::printAllMoves() {

	std::cout << "Legal Moves" << std::endl;
	std::cout << white_move << std::endl;
	for (MoveSet moveSet : legalMoves) {

		

		for (auto end : moveSet.moves) {
			std::array<int, 2> start = { end.p->getRow(), end.p->getCol() };

			std::cout << "Piece(" << end.p->getType() << "):   " << '(' << start[0] << ", " << start[1] << ") -->" << '(' << end.r << ", " << end.c << ')' << end.fashion<< endl;

		}
	};

}

void ClassicChess::printBoard() {
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
}
//SET UP-------------

Piece* ClassicChess::storePiece(int r, int c, PieceType type) {

	// assume row greater than 4 is white
	// this is where we can store differnt objects based on type but for not itll be on Piece class later can do inhertiance

	bool upper = r < BOARDROWS / 2;
	bool is_white = (upper == white_upper);

	Piece piece = Piece(r, c, is_white, type, board);

	if (is_white) {
		whitePieces.push_back(piece);
		return &(whitePieces.back());
	}
	else {
		blackPieces.push_back(piece);
		return &(blackPieces.back());
	}

}

void ClassicChess::initClassicGame() {

	whitePieces.reserve(pieceNumber);
	blackPieces.reserve(pieceNumber);

	for (int r{}; r < BOARDROWS; r++) {

		for (int c{}; c < BOARDCOLS; c++) {

			if (r == 1 || r == 6) {
				board[r][c] = storePiece(r, c, Pawn);
			}
			else if (r == 0 || r == 7) {
				if (c == 0 || c == 7) {
					board[r][c] = storePiece(r, c, Rook);
				}
				else if (c == 1 || c == 6) {
					board[r][c] = storePiece(r, c, Knight);
				}
				else if (c == 2 || c == 5) {
					board[r][c] = storePiece(r, c, Bishop);
				}
				else if (c == 3) {
					board[r][c] = storePiece(r, c, Queen);
				}
				else if (c == 4) {
					board[r][c] = storePiece(r, c, King);
					if ((board[r][c])->getColor()) {
						this->whiteKing = board[r][c];
					}
					else {
						this->blackKing = board[r][c];
					}
				}
			}
		};
	};

}

//BOARD/PIECE OPERATIONS ---------------------

// updates the board with a move and returns a move record
ClassicChess::MoveRecord ClassicChess::final_move(const MoveEndpoint& move ) {

			const int ogR = move.p->getRow();
			const int ogC = move.p->getCol();
			const int newR = move.r;
			const int newC = move.c;

			MoveRecord record;
			record.end = move;


			if (move.fashion == EN_PASSENT) {
				record.taken = board[ogR][newC];
			}
			else {
				record.taken = board[newR][newC];
			}

			record.moved = move.p;
			record.startRow = ogR;
			record.startCol = ogC;

			if (move.fashion == EN_PASSENT) {
				if (record.taken) {
					record.taken->captured = true;
				}

				board[ogR][newC] = nullptr; // remove captured pawn
				board[ogR][ogC] = nullptr;  // clear old square

				move.p->incrementMove();
				move.p->move(newR, newC);
				board[newR][newC] = move.p;

				return record;
			}

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
					board[ogR][ogC + 2] = move.p;
					move.p->move(ogR, ogC + 2);
					move.p->incrementMove();

					board[ogR][ogC + 2 - 1] = rookToCastle;
					rookToCastle->move(ogR, ogC + 2 - 1);
					rookToCastle->incrementMove();

				}
				else {
					// rook is to the left of the king
					board[ogR][ogC - 2] = move.p;
					move.p->move(ogR, ogC - 2);
					move.p->incrementMove();

					board[ogR][ogC - 2 + 1] = rookToCastle;
					rookToCastle->move(ogR, ogC - 2 + 1);
					rookToCastle->incrementMove();

				}

				return record;
			}


			if (board[newR][newC]) {
				board[newR][newC]->captured = true;
			}


			board[ogR][ogC]->incrementMove();
			board[ogR][ogC]->move(newR, newC);
			board[newR][newC] = board[ogR][ogC];

			board[ogR][ogC] = nullptr;

			if (move.fashion == PAWN_PROMOTION) {
				move.p->changeType(PieceType::Queen);
			}

			if (move.fashion == EN_PASSENT) {

				board[ogR][newC] = nullptr;

			};
			
			// auto empties old spot isnt accurate for castling

			return record;
			
		};

// updates the board by undoing a move from its record
void ClassicChess::undo_move(MoveRecord record) {
			
			
			const auto p = record.moved;
			const int newR = record.end.r;
			const int newC = record.end.c;
			const int oldR = record.startRow;
			const int oldC = record.startCol;
			const MoveFashion fashion = record.end.fashion;

			if (fashion == EN_PASSENT) {
				board[newR][newC] = nullptr;

				board[oldR][newC] = record.taken;
				if (record.taken) {
					record.taken->captured = false;
				}


				board[oldR][oldC] = p;
				p->move(oldR, oldC);
				p->deincrementMove();



				return;
			};

			if (fashion == CASTLE && record.taken) {
				//remove the caslte things
				board[newR][newC] = record.taken;
				board[oldR][oldC] = p;
				p->move(oldR, oldC);
				p->deincrementMove();

				// in this case the taken is the rook that moved
				record.taken->move(newR, newC);
				record.taken->deincrementMove();

				// clear the moved spots
				if (newC > oldC) {
					// rook is to the right of the king
					board[oldR][oldC + 2] = nullptr;

					board[oldR][oldC + 2 - 1] = nullptr;

				}
				else {
					// rook is to the right of the king
					board[oldR][oldC - 2] = nullptr;


					board[oldR][oldC - 2 + 1] = nullptr;

				}

				return;
			}

			if (fashion == PAWN_PROMOTION) {

				if (record.taken) {
					record.taken->captured = false;
				}

				p->changeType(Pawn);
				board[newR][newC] = record.taken;
				board[oldR][oldC] = p;
				p->move(oldR, oldC);
				p->deincrementMove();

				return;

			}

			// might laso just encompass undo castle logic
			if (fashion == STANDARD) {

				if (record.taken) {
					record.taken->captured = false;
				}

				board[newR][newC] = record.taken;
				board[oldR][oldC] = p;
				p->move(oldR, oldC);
				p->deincrementMove();

				return;
			}

		}

//MOVE GENERATION------------

//directional vector
static int directions[8][2] = {

		{0, -1},
		{0, 1},
		{-1, 0},
		{1, 0},
		{-1,-1},
		{-1, 1},
		{1,-1}, 
		{1, 1}

	};

//knight movement
static int knightOffsets[8][2] {

		{1, -2},
		{1, 2},
		{-1, -2},
		{-1, 2},
		{2, -1},
		{2, 1},
		{-2, -1},
		{-2, 1}

};

//check if king sqaure is attacked
bool ClassicChess::is_checked(bool is_white) {
	
	// horizontal right
	Piece* king;

	if (is_white) {
		king = whiteKing;
	} else {
		king = blackKing;
	}

	int r = king->getRow();
	int c = king->getCol();
	
	if (is_attacked(r, c, is_white)) {
		return true;
	}

	return false;
}

//check if a certain square is attacked
bool ClassicChess::is_attacked(int r, int c, bool is_white) {
	
	PieceType enemy;

	// check all directions for enemy rook + queen + rook || also check for pawn and king on first tierations
	for (auto vector : directions) {
		
		if ((vector[0] == 0) || (vector[1] == 0)) {
			//straight
			enemy = Rook;
		} else {
			//diagonal
			enemy = Bishop;
		}

		//printBoard();
		int j = 1;

		while (true) {
			// check if out of bounds
			
			int row = r + (vector[0]*j);
			int col = c + (vector[1]*j);

			//std::cout<<j<<row<<col<<"\n";
			
			if (((col) < 0 || (row) < 0) || ((col) > 7|| (row) > 7) ) {
				break;
			}

			auto piece = board[row][col];
				
			if (piece) {
				if (piece->getColor() == is_white) {
					// check if piece is same colors
					break;
						
				} else {
					if (piece->getType() == enemy || piece->getType() == Queen)  {
					
						return true;
					}
					break;
				}
			};

			j++;
		}
	}

	// check for knight 
	for (auto offset : knightOffsets) {
		int row = r + offset[0];
		int col = c + offset[1];

		if (((col) < 0 || (row) < 0) || ((col) > 7 || (row) > 7)) {
			continue;
		}

		auto piece = board[row][col];

		if (piece) {
			if ((piece->getColor() != is_white) && (piece->getType() == Knight)) {
				// check if piece is same colors
			
				return true;

			}
		};
	}

	// check for king and pawn
	//std::cout << "______"<< "\n";
	for (auto vector : directions) {
		
		int row = r + vector[0];
		int col = c + vector[1];

		if (((col) < 0 || (row) < 0) || ((col) > 7 || (row) > 7)) {
			continue;
		}

		
		
		auto piece = board[row][col];
		//check for king
		if (piece) {
			if (piece->getColor() == is_white) {
				// check if piece is same colors
				continue;
			}
			else {
				if (piece->getType() == King) {
					
					return true;
				};
				
				if (piece->getType() == Pawn) {
					
					//piece->toString();
					
					if ((vector[0] != 0) && (vector[1] != 0)) {
						
						
						if (is_white == white_upper) {
							//upper
							
							if (vector[0] > 0) {
								
								return true;
							}

						}
						else {
							//lower
							
							if (vector[0] < 0) {
								
								return true;
							}

						}

					}



				}

			}
				
		}

	}
	return false;

}

//check if its possible for a piece to be a pinned piece
bool ClassicChess::is_pinned(Piece& p) {

	Piece* king = p.getColor() ? whiteKing : blackKing;

	int kingR = king->getRow();
	int kingC = king->getCol();


	if (p.getRow() == kingR) {
		return true;
	}

	if (p.getCol() == kingC) {
		return true;
	}

	if (abs(p.getCol() - p.getRow()) == abs(kingC - kingR)) {
		return true;
	}

	return false;


}

//generate all pseudo legal moves, returns a moveSet, of a piece and its moves
std::vector<ClassicChess::MoveSet> ClassicChess::getPseudoMoves(std::vector<Piece>& pieces) {
	
	std::vector<MoveSet> pseudo_moves;

	for (Piece& p : pieces) {

		if (p.captured) {
			continue;
		}

		MoveSet move;
		auto moves = p.pseudoLegalMoves();

		for (auto end : moves) {

			MoveEndpoint e;
			e.r = end[0];
			e.c = end[1];
			e.p = &p;

			// pawn promotion
			if (p.getType() == Pawn && (e.r == 0 || e.r == 7)) {
				e.fashion = PAWN_PROMOTION;
			}
			else {
				e.fashion = STANDARD;
			}

			move.moves.emplace_back(e);

		}

		// castling
		if (p.getType() == King && p.getTimesMoved() == 0) {

			for (auto end : p.getCastledMoves()) {

				MoveEndpoint e;
				e.r = end[0];
				e.c = end[1];
				e.fashion = CASTLE;
				e.p = &p;
				move.moves.emplace_back(e);

			}
		}

		if (p.getType() == Pawn && ((p.getTimesMoved() == 2 || p.getTimesMoved() == 3))) {
			
			for (auto end : p.getEnPassent()) {
				MoveEndpoint e;
				e.r = end[0];
				e.c = end[1];
				e.fashion = EN_PASSENT;
				e.p = &p;
				move.moves.emplace_back(e);
			}

		}

		pseudo_moves.emplace_back(move);
	}
	return pseudo_moves;
}

//calls is_pinned + is_checked on a moveSet to see if it is LEGAL by simulating the move and then undoing it
void ClassicChess::filterMoveSet(ClassicChess::MoveSet& move, bool kingInCheck, Piece* piece) {

	std::array<int,2> start = { piece->getRow() , piece->getCol() };

	for (int i{ static_cast<int>(move.moves.size()) }; --i >= 0;) {

		auto end = move.moves[i];

		//handle check legality
		MoveRecord record = final_move(move.moves[i]);
		bool check = is_checked(piece->getColor());
		undo_move(record);

		

		//handle castle legality
		bool illegal = false;
		if (end.fashion == CASTLE) {
			if (kingInCheck) {
				illegal = true;
			}
			else {
				if (end.c > start[1]) {
					if (is_attacked(start[0], start[1] + 1, piece->getColor())) {
						illegal = true;
					}
				}
				else {
					if (is_attacked(start[0], start[1] - 1, piece->getColor())) {
						illegal = true;
					}
				}
			}
		}

		if (check || illegal) {
			
			move.moves[i] = move.moves.back();
			move.moves.pop_back();

		}

	}


}

//GAME SEQUENCE------------

//checks if there are legal moves available
bool ClassicChess::hasLegalMoves() {
	for (const auto& batch : legalMoves) {
		if (!batch.moves.empty()) {
			return true;
		}
	}
	return false;
}

//calcaultes game state
ClassicChess::OutCome ClassicChess::calculateState() {
	if (hasLegalMoves()) {
		return Normal;
	}

	if (white_move) {
		return is_checked(true) ? BlackWin : Draw;
	}
	else {
		return is_checked(false) ? WhiteWin : Draw;
	}
}

//validate user input to pick
bool ClassicChess::verifyPick(int r, int c){
	
	if ((r>=8 || r < 0) || (c>=8 || c < 0)) {
		return false;
	}
	
	for (auto& moveSet : legalMoves) {
		for (auto& move : moveSet.moves) {

			if (move.p == board[r][c]) {

				return true;
			}
		}
	}
	
	
	return false;

}

//validate user input to pick to move
std::variant<bool, MoveEndpoint> ClassicChess::verifyMove(int r, int c, Piece* piece){

	if ((r>=8 || r < 0) || (c>=8 || c < 0)) {
		return false;
	}

	

	for (auto &moveSet: this->legalMoves) {

				
		for (auto move : moveSet.moves) {

				
			if (((move.r == r) && (move.c == c)) && (piece == move.p)){
				return move;
			}
		}
	} 
	
	return false;

}

//player turn
bool ClassicChess::move_turn() {

	int req_row;
	int req_col;

	std::cout<<"Pick Piece:"<<std::endl;
	if (white_move) {
		std::cout << "WHITE MOVE" << std::endl;
	}
	else {
		std::cout << "BLACK" << std::endl;
	}

	std::cout<<"row : ";
	std::cin>>req_row;

	std::cout<<"col : ";
	std::cin>>req_col;

	bool pick = verifyPick(req_row, req_col);
	while (!pick) {

		std::cout<<"please Pick a valid Piece:"<<std::endl;

		std::cout<<"row : ";
		std::cin>>req_row;

		std::cout<<"col : ";
		std::cin>>req_col;

		pick = verifyPick(req_row, req_col);
		
	}

	Piece* piece = ClassicChess::board[req_row][req_col];
	// pick works

	int move_row;
	int move_col;

	std::cout<<"Move Piece:"<<std::endl;

	piece->toString();

	std::cout<<"row : ";
	std::cin>>move_row;

	std::cout<<"col : ";
	std::cin>>move_col;

	std::variant<bool, MoveEndpoint> move = verifyMove(move_row, move_col, piece);
	while (std::holds_alternative<bool>(move)) {

		std::cout<<"Please pick a valid move:"<<std::endl;

		std::cout<<"row : ";
		std::cin>>move_row;

		std::cout<<"col : ";
		std::cin>>move_col;
		
		move = verifyMove(move_row, move_col, piece);
	}


	//happens if eveyrhing is succesful
	MoveEndpoint fmove = std::get<MoveEndpoint>(move);
	this->final_move(fmove);
	
	return true;


};

//regular pvp gameloop
void ClassicChess::gameLoop() {

	initClassicGame();

	while (this->game) {

		std::cout << "VALUE OF BOARD:" << evaluateBoard() << std::endl;
		printBoard();





		legalMoves = GenerateOrderedLegalMoves(white_move, MoveEndpoint{}, false);
		printAllMoves();

		auto outCome = calculateState();
		std::cout <<"outcome : "<< outCome << endl;
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
		bool turn = move_turn();
		if (turn) {

			iterator += 1;
			if (white_move) {
				white_move = false;
			}
			else {
				white_move = true;
			}
		}

		// calgulare the state
	}
}

//ai vs player gameloop
void ClassicChess::gameLoopVSminimaxAI(bool whiteIsAi, int depth) {

	initClassicGame();
	initZobrist();


	while (this->game) {

		std::cout << "VALUE OF BOARD:" << evaluateBoard() << std::endl;
		printBoard();


		uint64_t key = getHashCode(white_move);
		TTEntry& cached = transpositionalTable[key % TTsize];

		bool hasTT = (key == cached.id);

		MoveEndpoint orderTT{};
		if (hasTT) {
			orderTT = cached.move;
		}



		legalMoves = GenerateOrderedLegalMoves(white_move, orderTT, hasTT);
		printAllMoves();

		auto outCome = calculateState();
		std::cout << "outcome : " << outCome << endl;
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
			final_move(bestMove.move);


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


		// calgulare the state
	}
}



