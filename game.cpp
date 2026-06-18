#include "game.h"
#include "vector"
#include <iostream>
#include <variant>

//DEBUG----------------------
void ClassicChess::printAllMoves() {

	std::cout << "White Moves" << std::endl;
	for (MoveSet moveSet : legalWhiteMoves) {

		std::array<int,2> start = { moveSet.piece->getRow(), moveSet.piece->getCol() };

		for (auto end : moveSet.moves) {

			std::cout << "Piece(" << moveSet.piece->getType() << "):   " << '(' << start[0] << ", " << start[1] << ") -->" << '(' << end.r << ", " << end.c << ')' << end.fashion<< endl;

		}
	};

	std::cout << "Black Moves" << std::endl;
	for (MoveSet moveSet : legalBlackMoves) {

		std::array<int,2> start = { moveSet.piece->getRow(), moveSet.piece->getCol() };

		for (auto end : moveSet.moves) {

			std::cout << "Piece(" << moveSet.piece->getType() << "):   " << '(' << start[0] << ", " << start[1] << ") -->" << '(' << end.r << ", " << end.c << ')' << end.fashion<<endl;

		}

	}

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

//MOVE GENERATION------------

//vectors
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

// checks if current board state is check for the is_white. false it checks if black is checked for true it checks for white
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
						
				} else if (piece->getType() == enemy || piece->getType() == Queen)  {
					
					return true;
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
					std::cout << "Check by king" << "\n";
					return true;
				};
				
				if (piece->getType() == Pawn) {
					std::cout << "enemy pawn near by" << "\n";
					//piece->toString();
					std::cout << vector[0] << vector[1]<< "\n";
					if ((vector[0] != 0) && (vector[1] != 0)) {
						std::cout << "enemy pawn near by diag" << "\n";
						
						if (is_white == white_upper) {
							//upper
							std::cout << "upper " << "\n";
							if (vector[0] > 0) {
								std::cout << "Check by pawn " << "\n";
								return true;
							}

						}
						else {
							//lower
							std::cout << "lower " << "\n";
							if (vector[0] < 0) {
								std::cout << "Check by pawn " << "\n";
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

// check if a piece is in same row / col or diag as its king.
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

// iterates over whitePieces to get all PSEUDO moves
std::vector<ClassicChess::MoveSet> ClassicChess::getWhitePseudoMoves() {
	std::vector<MoveSet> white_moves;

	for (Piece& p : whitePieces) {
		if (p.captured) {
			continue;
		}

		MoveSet move;
		move.piece = &p;
		auto moves = p.pseudoLegalMoves();

		for (auto end : moves) {
			
			// where move details get generated

			MoveEndpoint e;
			e.r = end[0];
			e.c = end[1];

			// pawn promotion
			if (p.getType() == Pawn && (e.r == 0 || e.r== 7)) {
				e.fashion = PAWN_PROMOTION;
			} else {
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
				move.moves.emplace_back(e);

			}
		}

		white_moves.emplace_back(move);
	}

	return white_moves;


}

// iterates over whitePieces to get all PSEUDO moves
std::vector<ClassicChess::MoveSet> ClassicChess::getBlackPseudoMoves() {
	std::vector<MoveSet> black_moves;

	for (Piece& p : blackPieces) {

		if (p.captured) {
			continue;
		}

		MoveSet move;
		move.piece = &p;
		auto moves = p.pseudoLegalMoves();

		for (auto end : moves) {
			
			MoveEndpoint e;
			e.r = end[0];
			e.c = end[1];

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
				move.moves.emplace_back(e);

			}
		}

		black_moves.emplace_back(move);
	}
	return black_moves;
}

// calls is_pinned + is_checked on a moveSet to see if it is LEGAL by simulating the move and then undoing it
void ClassicChess::filterMoveSet(ClassicChess::MoveSet& move) {

	std::array<int,2> start = { move.piece->getRow() , move.piece->getCol() };

	for (int i{ static_cast<int>(move.moves.size()) }; --i >= 0;) {

		auto end = move.moves[i];
		Piece* taken = board[end.r][end.c];

		final_move(move.piece, end);

		bool check = is_checked(move.piece->getColor());
		
		undo_move(move.piece, end, taken, start);


		if (check) {
			

			move.moves[i] = move.moves.back();
			move.moves.pop_back();

		}

	}


}

// populates legalBlackMoves and legalWhiteMoves
void ClassicChess::generateLegalMoves() {
	legalWhiteMoves.clear();
	legalBlackMoves.clear();
	
	if (white_move) {
		//legalWhiteMoves.clear();
		auto whiteMoves = getWhitePseudoMoves();
		bool isWhiteChecked = is_checked(true);

		
		for (int i{}; i < whiteMoves.size(); i++) {

			if (isWhiteChecked) {
				filterMoveSet(whiteMoves[i]);
				if (whiteMoves[i].moves.size() == 0) {
					continue;
				}
				else {
					legalWhiteMoves.push_back(whiteMoves[i]);
				}
			}
			else {
				if (!is_pinned((*whiteMoves[i].piece))) {
					// cant affect king if moved
					legalWhiteMoves.push_back(whiteMoves[i]);

				}
				
				else {
					// if it is a possible pin then check if move will result in check
					filterMoveSet(whiteMoves[i]);

					if (whiteMoves[i].moves.size() == 0) {
						continue;

					}
					else {
						legalWhiteMoves.push_back(whiteMoves[i]);
					}
				}
			}
		}

	} else {
		//legalBlackMoves.clear();
		auto blackMoves = getBlackPseudoMoves();
		bool isBlackChecked = is_checked(false);
		for (int i{}; i < blackMoves.size(); i++) {
			if (isBlackChecked) {
				filterMoveSet(blackMoves[i]);

				if (blackMoves[i].moves.size() == 0) {
					continue;
				}
				else {
					legalBlackMoves.push_back(blackMoves[i]);
				}
			}
			else {
				if (!is_pinned(*blackMoves[i].piece)) {
					legalBlackMoves.push_back(blackMoves[i]);

				}
				else {
					// if it is a possible pin then check if move will result in check
					filterMoveSet(blackMoves[i]);

					if (blackMoves[i].moves.size() == 0) {
						continue;
					}
					else {
						legalBlackMoves.push_back(blackMoves[i]);
					}
				}
			}
		}
	}

}

//GAME SEQUENCE------------

bool ClassicChess::verifyPick(int r, int c){

	if ((r>=8 || r < 0) || (c>=8 || c < 0)) {
		return false;
	}
	
	
	if (white_move) {

		for (auto &moveInfo : legalWhiteMoves) {

			if (moveInfo.piece == board[r][c]) {
				return true;
			}

		}
	}
	else {
		for (auto &moveInfo : legalBlackMoves) {

			if (moveInfo.piece == board[r][c]) {
				return true;
			}

		}
	}
	
	return false;

}

std::variant<bool, MoveEndpoint> ClassicChess::verifyMove(int r, int c, Piece* piece){

	if ((r>=8 || r < 0) || (c>=8 || c < 0)) {
		return false;
	}

	
	if (white_move) {
		for (auto &move: this->legalWhiteMoves) {

			if (move.piece == piece) {
				
				for (auto coords : move.moves) {

					std::cout << r << c << coords.r << coords.c << std::endl;
					if ((coords.r == r) && (coords.c == c)) {
						return coords;
					}
				}
			} 

		}
	}
	else {

		for (auto &move : this->legalBlackMoves) {

			if (move.piece == piece) {
				
				for (auto coords : move.moves) {

					std::cout << r << c << coords.r << coords.c << std::endl;
					if ((coords.r == r) && (coords.c == c)) {
						return coords;
					}
				}
			}

		}
	}
	
	return false;

}

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
	this->final_move(piece, fmove);
	
	return true;


};

void ClassicChess::gameLoop() {

	initClassicGame();

	while (this->game) {


		printBoard();
		generateLegalMoves();
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










