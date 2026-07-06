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
ClassicChess::EvaluatedMove ClassicChess::getBestMoveIterative(int maxDepth, bool whiteToMove) {
    resetSearchStats();

    auto start = std::chrono::high_resolution_clock::now();

    EvaluatedMove best{};

    for (int depth = 1; depth <= maxDepth; depth++) {
        uint64_t key = getHashCode(whiteToMove);
        TTEntry& cached = transpositionalTable[key % TTsize];

        bool hasTT = cached.id == key;
        MoveEndpoint ttMove{};

        if (hasTT) {
            ttMove = cached.move;
        }

        legalMoves = GenerateOrderedLegalMoves(whiteToMove, ttMove, hasTT);

        best = searchRoot(depth, whiteToMove);

        TTEntry rootEntry{};
        rootEntry.depth = depth;
        rootEntry.whitemove = whiteToMove;
        rootEntry.bound_type = EXACT;
        rootEntry.score = best.value;
        rootEntry.move = best.move;

        cacheEntryTT(rootEntry);
    }

    auto end = std::chrono::high_resolution_clock::now();

    stats.elapsedMs =
        std::chrono::duration<double, std::milli>(end - start).count();

    printSearchStats(maxDepth);

    return best;
}
// big issue array circular dependence
ClassicChess::EvaluatedMove ClassicChess::searchRoot(int depth, bool whiteToMove) {
    // moves already generated
    //resetSearchStats();
    //auto start = std::chrono::high_resolution_clock::now();


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
    //auto end = std::chrono::high_resolution_clock::now();

    //stats.elapsedMs =
        //std::chrono::duration<double, std::milli>(end - start).count();
    //printSearchStats(depth);

    return best;

}

int ClassicChess::minimax(int depth, bool whiteToMove, int alpha, int beta) {
    stats.nodes++;
    if (depth == 0) {
        stats.leafNodes++;
        return evaluateBoard();
    }


    //tt lookup
    uint64_t key = getHashCode(whiteToMove);
    TTEntry& cached = transpositionalTable[key % TTsize];
    MoveEndpoint orderTT{};

    if (cached.id == key) {
        //store TT move
        orderTT = cached.move;
        stats.ttHits++;
        if (cached.depth >= depth) {

            if (cached.bound_type == EXACT) {
                return cached.score;
            }

            if (cached.bound_type == LOWER_BOUND) {
                alpha = std::max(alpha, cached.score);
            }
            else if (cached.bound_type == UPPER_BOUND) {
                beta = std::min(beta, cached.score);
            }

            if (alpha >= beta) {
                return cached.score;
            }
        }
    }
    


    //tt cache start 
    TTEntry entry{};
    entry.depth = depth;
    entry.whitemove = whiteToMove;
   
    //generate moves
    // it will generate the moves for the side that is max or min depending on input
    auto legal_moves_node = GenerateOrderedLegalMoves(whiteToMove, orderTT, cached.id == key);
    int best;
    
    MoveEndpoint bestMove{};

    bool maximizing = (whiteToMove == whiteMaximizing);

    if (maximizing) {

        
        bool hasMoves = false;

        for (const auto& batch : legal_moves_node) {
            if (!batch.moves.empty()) {
                hasMoves = true;
                break;
            }
        }

        // dont need to cache these
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
                    bestMove = endpoint;
                }           

                if (best > alpha) {
                    alpha = best;
                }

                if (alpha >= beta) {
                    stats.alphaBetaCutoffs++;


                    //  cache as lowerbound
                    entry.bound_type = LOWER_BOUND;
                    entry.score = best;
                    entry.move = endpoint;
                    cacheEntryTT(entry);
                    stats.ttStores++;

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
                    bestMove = endpoint;
                }

                if (best < beta) {
                    beta = best;
             
                }

                if (beta <= alpha) {
                    stats.alphaBetaCutoffs++;


                    //  cache as upper bound
                    entry.bound_type = UPPER_BOUND;
                    entry.score = best;
                    entry.move = endpoint;
                    cacheEntryTT(entry);
                    stats.ttStores++;
                    return best;
                }


            }
        }

    }

    // cache as exact
    entry.bound_type = EXACT;
    entry.score = best;
    entry.move = bestMove;
    cacheEntryTT(entry);
    stats.ttStores++;
    return best;
}

// generate a unique hash from board state
uint64_t ClassicChess::getHashCode(bool whitemove) {
    uint64_t hash = 0;


    for (int r{}; r < 8; r++) {
        for (int c{}; c < 8; c++) {

            auto p = board[r][c];
            if (!p) {
                continue;
            }

            int square = r * 8 + c;
            int color = (p->getColor()) ? 1 : 0;
            int type = p->getType() - 1;

            hash ^= zobristID[square][color][type];
        }
    }

    if (whitemove) {
        hash ^= white_move_key;
    }

    return hash;
};

// later use bucketed to opt if needed
// also i need to ensure 
void ClassicChess::cacheEntryTT(TTEntry entry) {
    uint64_t key = getHashCode(entry.whitemove);
    int index = key % TTsize;

    entry.id = key;


    if ((transpositionalTable[index].id == key) && transpositionalTable[index].depth > entry.depth) {
        return;
    };

    transpositionalTable[index] = entry;

};

// will decipher whether or not move is one of TT, capture, km, quiet
// if capture, befor the function returns it will by ref change the value of the move object
ClassicChess::MoveBunch ClassicChess::analyzeMove(MoveEndpoint& move, const MoveEndpoint TTmove, bool isTT) {


    if (isTT) {

        if (sameMove(move, TTmove)) {

            return TT;


        }



    }


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

std::array<ClassicChess::MoveSet,4> ClassicChess::GenerateOrderedLegalMoves(bool is_white, const MoveEndpoint TTmove, bool isTT) {

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

                MoveBunch category = analyzeMove(move, TTmove, isTT);

                orderedLegalMoves[category].moves.push_back(move);

            }            
        }        
        
    }

    // order captures, rest dont need ordering since ihavent implemented them yet
    auto CompareByValue = [](const MoveEndpoint& a, const MoveEndpoint& b) {
        return a.value > b.value;
        };

    std::sort(orderedLegalMoves[CAPTURES].moves.begin(), orderedLegalMoves[CAPTURES].moves.end(), CompareByValue);


    return orderedLegalMoves;


}