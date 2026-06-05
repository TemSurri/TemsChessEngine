#include "game.h"
#include "vector"
#include <iostream>

//SET UP

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

//MOVE GENERATION

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


// changing the current system from expenisve look into the future to using an outward king detection and a pin check.
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
	
	PieceType enemy;

	// check all directions for enemy rook + queen + rook
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
					std::cout<<"Check"<<"\n";
					return true;
				}
			};

			j++;
		}
	}


	//gotta check for knight + king + pawn
	
	
	

}




bool ClassicChess::is_pinned(Piece& p) {

	int kingR = p.getColor() ? (whiteKing->getRow()) : (blackKing->getRow());
	int kingC = p.getColor() ? (whiteKing->getCol()) : (blackKing->getCol());


	if (p.getRow() == kingR) {
		return true;
	}

	if (p.getCol() == kingC) {
		return true;
	}

	if ((p.getCol() - p.getRow()) == (kingR - kingC)) {
		return true;
	}

	return false;


}
bool ClassicChess::check(bool for_white) {
	 
	
	if (for_white) {
		auto black_moves = getBlackPseudoMoves();
		//whiteKing->toString();
		for (auto move : black_moves) {
			//std::cout << move.move[1][0] << move.move[1][1] << std::endl;
			if ((whiteKing->getRow() == move.move[1][0]) && (whiteKing->getCol() == move.move[1][1])) {
				return true;
			}
		}

		return false;
	}
	else {
		auto white_moves = getWhitePseudoMoves();
		for (auto move : white_moves) {
			if ((blackKing->getRow() == move.move[1][0]) && (blackKing->getCol() == move.move[1][1])) {
				return true;
			}
		}

		return false;
	}

};



//a function that moves the board and then undoes the move
bool ClassicChess::virtualMoveCauseCheck(MoveInfo move) {
	//std::cout<<"hello start of v move \n";
	// 1
	auto start = move.move[0];
	auto end = move.move[1];

	Piece* taken = board[end[0]][end[1]];



	//piece info update
	move.piece->move(end[0], end[1]);
	move.piece->incrementMove();
	board[end[0]][end[1]] = move.piece;
	board[start[0]][start[1]] = nullptr;
	if (taken) {
		taken->captured = true;
	}

	

	//get state
	bool state = is_checked(move.piece->getColor());
	//	
	
	//undo move
	if (taken) {
		taken->captured = false;
	}
	board[end[0]][end[1]] = taken;
	board[start[0]][start[1]] = move.piece;
	move.piece->move(start[0], start[1]);
	move.piece->deincrementMove();
	
	//std::cout<<"end of v move \n";
	//return state
	return !state;

}
std::vector<ClassicChess::MoveInfo> ClassicChess::getWhitePseudoMoves() {
	std::vector<MoveInfo> white_moves;

	for (Piece& p : whitePieces) {
		if (p.captured) {
			continue;
		}

		std::array<int, 2> start_pos = { p.getRow(), p.getCol() };

		auto moves = p.pseudoLegalMoves();

		for (auto end : moves) {
			MoveInfo move;
			move.piece = &p;
			move.move = { start_pos, end };
			white_moves.emplace_back(move);
		}
	}

	return white_moves;


}
std::vector<ClassicChess::MoveInfo> ClassicChess::getBlackPseudoMoves() {
	std::vector<MoveInfo> black_moves;

	for (Piece& p : blackPieces) {

		if (p.captured) {
			continue;
		}

		std::array<int, 2> start_pos = { p.getRow(), p.getCol() };


		auto moves = p.pseudoLegalMoves();

		for (auto end : moves) {
			MoveInfo move;
			move.piece = &p;
			move.move = { start_pos, end };
			black_moves.emplace_back(move);

		}
	}
	return black_moves;
}


//updates moves
void ClassicChess::generateLegalMoves() {

	legalBlackMoves.clear();
	legalWhiteMoves.clear();

	auto whiteMoves = getWhitePseudoMoves();
	auto blackMoves = getBlackPseudoMoves();

	//std::cout<<"hello \n";

	bool isWhiteChecked = is_checked(true);
	bool isBlackChecked = is_checked(false);


	std::cout<<isWhiteChecked<<isBlackChecked<< "\n";

	// filter white moves and add to legal moves
	for (int i{}; i < whiteMoves.size(); i++) {
		//std::cout << i << "\n";
		//std::cout << legalWhiteMoves.size() << "\n";

		if (isWhiteChecked) {
			// if check then immedietaly check if move will still have check
			bool legal = virtualMoveCauseCheck(whiteMoves[i]);
			//std::cout << "white checked" << "\n";
			if (!legal) {
				continue;
			}
			else {
				legalWhiteMoves.push_back(whiteMoves[i]);
			}
		}
		else {
			if (!is_pinned(*whiteMoves[i].piece)) {
				// cant affect king if moved
				//std::cout << "not pinned" << "\n";
				legalWhiteMoves.push_back(whiteMoves[i]);

			}
			
			else {
				// if it is a possible pin then check if move will result in check
				bool legal = virtualMoveCauseCheck(whiteMoves[i]);

				if (!legal) {
					//std::cout << "illegal" << "\n";
					continue;
				}
				else {
					//std::cout << "legal" << "\n";
					legalWhiteMoves.push_back(whiteMoves[i]);
				}
			}
		}
	}

	for (int i{}; i < blackMoves.size(); i++) {
		//std::cout << i << "\n";

		if (isWhiteChecked) {
			// if check then immedietaly check if move will still have check
			bool legal = virtualMoveCauseCheck(blackMoves[i]);

			if (!legal) {
				continue;
			}
			else {
				legalBlackMoves.push_back(blackMoves[i]);
			}
		}
		else {
			if (!is_pinned(*blackMoves[i].piece)) {
				// cant affect king if moved
				legalBlackMoves.push_back(blackMoves[i]);

			}

			else {
				// if it is a possible pin then check if move will result in check
				bool legal = virtualMoveCauseCheck(blackMoves[i]);

				if (!legal) {
					continue;
				}
				else {
					legalBlackMoves.push_back(blackMoves[i]);
				}
			}
		}
	}


	


}







//GAME SEQUENCE


bool ClassicChess::verifyPick(int r, int c){

	if ((r>=8 || r < 0) || (c>=8 || c < 0)) {
		return false;
	}
	
	
	if (board[r][c]) {
		
		if (board[r][c]->getColor() == white_move)
		{
			return true;
		}
	} 
	
	return false;

}

bool ClassicChess::verifyMove(int r, int c, Piece* piece){

	if ((r>=8 || r < 0) || (c>=8 || c < 0)) {
		return false;
	}

	
	if (white_move) {
		for (auto move: this->legalWhiteMoves) {

			if (move.piece == piece) {
				
				auto coords = move.move[1];
				std::cout<<r<<c<<coords[0]<<coords[1]<<std::endl;					
				if ((coords[0] == r) && (coords[1] == c)) {
					return true;
				}
			} 

		}
	}
	else {

		for (auto move : this->legalBlackMoves) {

			if (move.piece == piece) {
				
				auto coords = move.move[1];
				std::cout << r << c << coords[0] << coords[1] << std::endl;
				if ((coords[0] == r) && (coords[1] == c)) {
					return true;
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

	bool move = verifyMove(move_row, move_col, piece);
	while (!move) {

		std::cout<<"Please pick a valid move:"<<std::endl;

		std::cout<<"row : ";
		std::cin>>move_row;

		std::cout<<"col : ";
		std::cin>>move_col;
		
		move = verifyMove(move_row, move_col, piece);
	}


	//happens if eveyrhing is succesful
	this->real_move(req_row, req_col, move_row, move_col);
	
	return true;


};



ClassicChess::OutCome ClassicChess::gameLoop() {

	initClassicGame();

	while (this->game.active) {
		printBoard();
		generateLegalMoves();
		printAllMoves();

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
	}
	//for now
	return Draw;
}










