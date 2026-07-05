#include "game.h"
#include "gameInfo.h"
#include <array>
#include <algorithm>
#include <chrono>

// in the index order of my enum in 
constexpr std::array<int, 7> pieceValues = {
    0,
    100,
    320,
    330,
    500,
    900,
    20000
};

int ClassicChess::evaluateBoard() {
    int boardValue{};

    for (auto& p : whitePieces) {
        if (p.captured) {
            continue;
        }
        boardValue = (whiteMaximizing) ? (boardValue + pieceValues[p.getType()]) : (boardValue - pieceValues[p.getType()]);
    }

    for (auto& p : blackPieces) {
        if (p.captured) {
            continue;
        }
        boardValue = (whiteMaximizing) ? (boardValue - pieceValues[p.getType()]) : (boardValue + pieceValues[p.getType()]);
    }

    return boardValue;
}

// big issue array circular dependence
ClassicChess::EvaluatedMove ClassicChess::getBestMove(int depth, bool whiteToMove) {
    // moves already generated
    resetSearchStats();
    auto start = std::chrono::high_resolution_clock::now();


    int alpha = -100000;
    int beta = 100000;


    EvaluatedMove best;
    bool maximizing = (whiteToMove == whiteMaximizing);
    if (maximizing) {
        best.value = -100000;
        for (auto& move : legalMoves) {
            for (auto& endpoint : move.moves) {
                MoveRecord mrecord = final_move(endpoint);
                int cur_val = minimax(depth - 1, !whiteToMove, alpha, beta);
                undo_move(mrecord);

                if (cur_val > best.value) {
                    best = {
                        cur_val, endpoint
                    }; 
                }


                if (best.value > alpha) {
                    alpha = best.value;
                }

            }
        }

    }

    else {
        best.value = 100000;
        for (auto& move : legalMoves) {
            for (auto& endpoint : move.moves) {
                MoveRecord mrecord = final_move(endpoint);
                int cur_val = minimax(depth - 1, !whiteToMove, alpha, beta);

                undo_move(mrecord);

                if (cur_val < best.value) {
                    best = {
                        cur_val, endpoint
                    };
                }

                if (best.value < beta) {
                    beta = best.value;
                }

            }
        }

    }
    auto end = std::chrono::high_resolution_clock::now();

    stats.elapsedMs =
        std::chrono::duration<double, std::milli>(end - start).count();
    printSearchStats(depth);

    return best;

}

int ClassicChess::minimax(int depth, bool whiteToMove, int alpha, int beta) {
    stats.nodes++;

    if (depth == 0) {
        stats.leafNodes++;
        return evaluateBoard();
    }

   
    //generate moves
    // it will generate the moves for the side that is max or min depending on input
    auto legal_moves_node = GenerateOrderedLegalMoves(whiteToMove);
    int best;

    bool maximizing = (whiteToMove == whiteMaximizing);

    if (maximizing) {


        bool hasMoves = false;

        for (const auto& batch : legal_moves_node) {
            if (!batch.moves.empty()) {
                hasMoves = true;
                break;
            }
        }

        if (!hasMoves) {
            if (is_checked(whiteToMove)) {
                return -100000; //checkmated
            }
            return 0; // stalemate
        }

        //alpha = vsmall#
        best = -100000;
        //for move in moves
        //  do move
        //  beta = minimax(move, p, depth-1, !maximizing)
        //  undo move
        //  if beta is larger than alpha, beta is new alpha
        
        for (auto& move : legal_moves_node) {
            for (auto& endpoint : move.moves) {

                MoveRecord mrecord = final_move(endpoint);
                int cur_val = minimax(depth - 1, !whiteToMove, alpha, beta);
                undo_move(mrecord);


                if (cur_val > best) {
                    best = cur_val;
                }           

                if (best > alpha) {
                    alpha = best;
                }

                if (alpha >= beta) {
                    stats.alphaBetaCutoffs++;
                    return best;
                }

            }
        }

        

    }
    else {
        //alpha = vbig#
        // basicaly same as top

        bool hasMoves = false;

        for (const auto& batch : legal_moves_node) {
            if (!batch.moves.empty()) {
                hasMoves = true;
                break;
            }
        }

        if (!hasMoves) {
            if (is_checked(whiteToMove)) {
                return 100000; //checkmated
            }
            return 0; // stalemate
        }

        best = 100000;
       
        for (auto& move : legal_moves_node) {
            for (auto& endpoint : move.moves) {
                MoveRecord mrecord = final_move(endpoint);
                int cur_val = minimax(depth - 1, !whiteToMove, alpha, beta);
                undo_move(mrecord);

                if (cur_val < best) {
                    best = cur_val;
                }

                if (best < beta) {
                    beta = best;
                }

                if (beta <= alpha) {
                    stats.alphaBetaCutoffs++;
                    return best;
                }


            }
        }

       

    }
   
    return best;
}

// generate a unique hash from board state
uint64_t ClassicChess::getHashCode() {

    return 2;
};

// NEED TO IMPLEMENT IT IN THE MINIMAX : not yet though
void ClassicChess::cacheEntryTT(const MoveEndpoint& move, int score, int depth, bool maximizing, bool pruned) {

    TTEntry entry;
    entry.move = move;
    entry.depth = depth;
    entry.score = score;
    
    // logic for FLAG
    if (pruned) {
        if (maximizing) {
            entry.bound_type = LOWER_BOUND;
        }
        else {
            entry.bound_type = UPPER_BOUND;
        }
    }
    else {
        entry.bound_type = EXACT;
    }

    transpositionalTable[getHashCode()] = entry;

};

// will decipher whether or not move is one of TT, capture, km, quiet
// if capture, befor the function returns it will by ref change the value of the move object
ClassicChess::MoveBunch ClassicChess::analyzeMove(MoveEndpoint& move) {

    //check for TT : TODO not implemented yet
    

    //check for Capture
    Piece* taken = board[move.r][move.c];
    if (taken) {

        // capture
        // simple heuristic(aim for worse piece takes better), later make this call a heuristic function if i add more coplexity
        int value = pieceValues[taken->getType()]*5 - pieceValues[move.p->getType()];
        move.value = value;
        return CAPTURES;

    }
    else {

        //check if killer move : TODO not implemented yet




        //regular quiet move
        move.value = 0;
        return QUIET;
    }

   



}

std::array<ClassicChess::MoveSet,4> ClassicChess::GenerateOrderedLegalMoves(bool is_white) {

    bool isChecked = is_checked(is_white);
    auto pmoves = is_white ? (getPseudoMoves(whitePieces)) : (getPseudoMoves(blackPieces));

    std::array<ClassicChess::MoveSet, 4> orderedLegalMoves;


    for (int i{}; i < pmoves.size(); i++) {

        if (pmoves[i].moves.size() == 0) {
      
            continue;
        }

        if (isChecked || is_pinned((*pmoves[i].moves[0].p))) {
            filterMoveSet(pmoves[i], isChecked, pmoves[i].moves[0].p);
        }

        if (pmoves[i].moves.size() > 0) {
            // its a legal move
            for (auto& move : pmoves[i].moves) {

                MoveBunch category = analyzeMove(move);

                orderedLegalMoves[category].moves.push_back(move);

            }            
        }        
        
    }

    // now i have to order moves

    // order captures, rest dont need ordering since ihavent implemented them yet
    auto CompareByValue = [](const MoveEndpoint& a, const MoveEndpoint& b) {
        return a.value > b.value;
        };

    std::sort(orderedLegalMoves[CAPTURES].moves.begin(), orderedLegalMoves[CAPTURES].moves.end(), CompareByValue);


    return orderedLegalMoves;


}