#include "game.h"
#include <array>
#include <algorithm>
#include <chrono>
#include <bit>

//HEURISTICS -----------
// value of pieces
int ClassicChess::piece_value(PieceTypeBit piece) const
{
    switch (piece)
    {
    case W_PAWN:
    case B_PAWN: return 100;
    case W_KNIGHT:
    case B_KNIGHT: return 320;
    case W_BISHOP:
    case B_BISHOP: return 330;
    case W_ROOK:
    case B_ROOK: return 500;
    case W_QUEEN:
    case B_QUEEN: return 900;
    case W_KING:
    case B_KING: return 20000;
    default: return 0;
    }
}

// simple heurisitc for board value
int ClassicChess::evaluateBoard()
{
    int value = 0;

    value += std::popcount(w_pawns) * piece_value(W_PAWN);
    value += std::popcount(w_knights) * piece_value(W_KNIGHT);
    value += std::popcount(w_bishops) * piece_value(W_BISHOP);
    value += std::popcount(w_rooks) * piece_value(W_ROOK);
    value += std::popcount(w_queen) * piece_value(W_QUEEN);

    value -= std::popcount(b_pawns) * piece_value(B_PAWN);
    value -= std::popcount(b_knights) * piece_value(B_KNIGHT);
    value -= std::popcount(b_bishops) * piece_value(B_BISHOP);
    value -= std::popcount(b_rooks) * piece_value(B_ROOK);
    value -= std::popcount(b_queen) * piece_value(B_QUEEN);

    // central pawn presence
    uint64_t center = (1ULL << 27) | (1ULL << 28) | (1ULL << 35) | (1ULL << 36);
    value += std::popcount(w_pawns & center) * 40;
    value -= std::popcount(b_pawns & center) * 40;

    // punish early rook-pawn pushes
    if (!(w_pawns & (1ULL << 8)))  value -= 35; // a2 moved
    if (!(w_pawns & (1ULL << 15))) value -= 35; // h2 moved

    if (!(b_pawns & (1ULL << 48))) value += 35; // a7 moved
    if (!(b_pawns & (1ULL << 55))) value += 35; // h7 moved

    // reward d/e pawns leaving starting square
    if (!(w_pawns & (1ULL << 11))) value += 25; // d2 moved
    if (!(w_pawns & (1ULL << 12))) value += 25; // e2 moved

    if (!(b_pawns & (1ULL << 51))) value -= 25; // d7 moved
    if (!(b_pawns & (1ULL << 52))) value -= 25; // e7 moved

    // discourage early queen movement
    if (!(w_queen & (1ULL << 3)))
        value -= 80; // white queen left d1

    if (!(b_queen & (1ULL << 59)))
        value += 80; // black queen left d8

    return whiteMaximizing ? value : -value;
}

//SEARCH PROCESS -----------------

// wraps root search for iterative deepening
ClassicChess::EvaluatedMove ClassicChess::getBestMoveIterative(int maxDepth, bool whiteToMove) {
    clearKillerMoves();
    resetSearchStats();

    auto start = std::chrono::high_resolution_clock::now();

    EvaluatedMove best{};

    for (int depth = 1; depth <= maxDepth; depth++) {
        uint64_t key = getHashCode(whiteToMove);
        TTEntry& cached = transpositionalTable[key & (TTsize - 1)];

        bool hasTT = cached.id == key;
        Move ttMove{};

        if (hasTT) {
            ttMove = cached.move;
        }

        auto legalMoves = GenerateOrderedLegalMoves(whiteToMove, ttMove, hasTT, depth);

        best = searchRoot(depth, whiteToMove, legalMoves);

        TTEntry rootEntry{};
        rootEntry.depth = depth;
        rootEntry.whitemove = whiteToMove;
        rootEntry.bound_type = EXACT;
        rootEntry.score = best.value;
        rootEntry.id = key;
        rootEntry.move = best.move;

        cacheEntryTT(rootEntry);
    }

    auto end = std::chrono::high_resolution_clock::now();

    stats.elapsedMs =
        std::chrono::duration<double, std::milli>(end - start).count();

    printSearchStats(maxDepth);

    return best;
}

// initial root search that begins recrusive search
ClassicChess::EvaluatedMove ClassicChess::searchRoot(int depth, bool whiteToMove, const std::array<ClassicChess::MoveSet, 4>& legalMoves) {

    int alpha = -100000;
    int beta = 100000;


    EvaluatedMove best;
    bool maximizing = (whiteToMove == whiteMaximizing);
    if (maximizing) {
        best.value = -100000;
        for (auto& move : legalMoves) {
            for (auto& endpoint : move.moves) {
                MoveRecord mrecord = exec_move(endpoint);
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
                MoveRecord mrecord = exec_move(endpoint);
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

    return best;

}

// recursively searches at a depth to find best move
int ClassicChess::minimax(int depth, bool whiteToMove, int alpha, int beta) {
    stats.nodes++;
    if (depth == 0) {
        stats.leafNodes++;
        return evaluateBoard();
    }


    //tt lookup
    uint64_t key = getHashCode(whiteToMove);
    TTEntry& cached = transpositionalTable[key & (TTsize - 1)];
    Move orderTT{};
    

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
    auto legal_moves_node = GenerateOrderedLegalMoves(whiteToMove, orderTT, cached.id == key, depth);
    int best;
    
    Move bestMove{};

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
            if (is_king_in_check(whiteToMove)) {
                return -100000; //checkmated
            }
            return 0; // stalemate
        }
        
        best = -100000;           

        for (auto& move : legal_moves_node) {
            for (auto& endpoint : move.moves) {

                MoveRecord mrecord = exec_move(endpoint);

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

                    // store killer move
                    storeKillerMove(endpoint, depth);

                    // cache as lowerbound
                    entry.bound_type = LOWER_BOUND;
                    entry.score = best;
                    entry.id = key;
                    entry.move = endpoint;
                    cacheEntryTT(entry);
                    stats.ttStores++;

                    return best;
                }

            }
        }
    }
    else {
        
        bool hasMoves = false;

        for (const auto& batch : legal_moves_node) {
            if (!batch.moves.empty()) {
                hasMoves = true;
                break;
            }
        }

        if (!hasMoves) {
            if (is_king_in_check(whiteToMove)) {
                return 100000; //checkmated
            }
            return 0; // stalemate
        }

        best = 100000;
       
        for (auto& move : legal_moves_node) {
            for (auto& endpoint : move.moves) {
                MoveRecord mrecord = exec_move(endpoint);
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
                    
                    // store killer move
                    storeKillerMove(endpoint, depth);

                    //  cache as upper bound
                    entry.bound_type = UPPER_BOUND;
                    entry.score = best;
                    entry.id = key;
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
    entry.id = key;
    cacheEntryTT(entry);
    stats.ttStores++;
    return best;
}

//TT TABLE ----------
void ClassicChess::hash_piece_board(
    uint64_t pieces,
    int color,
    int pieceIndex,
    uint64_t& hash
)
{
    while (pieces)
    {
        int square = pop_lsb(pieces);
        hash ^= zobristID[square][color][pieceIndex];
    }
}

// generate a unique hash from board state
uint64_t ClassicChess::getHashCode(bool whiteToMove)
{
    uint64_t hash = 0;

    // white = color 1
    hash_piece_board(w_pawns, 1, 0, hash);
    hash_piece_board(w_knights, 1, 1, hash);
    hash_piece_board(w_bishops, 1, 2, hash);
    hash_piece_board(w_rooks, 1, 3, hash);
    hash_piece_board(w_queen, 1, 4, hash);
    hash_piece_board(w_king, 1, 5, hash);

    // black = color 0
    hash_piece_board(b_pawns, 0, 0, hash);
    hash_piece_board(b_knights, 0, 1, hash);
    hash_piece_board(b_bishops, 0, 2, hash);
    hash_piece_board(b_rooks, 0, 3, hash);
    hash_piece_board(b_queen, 0, 4, hash);
    hash_piece_board(b_king, 0, 5, hash);

    if (whiteToMove)
        hash ^= white_move_key;

    return hash;
}

// add entry to TT table
void ClassicChess::cacheEntryTT(TTEntry entry) {
  
    int index = entry.id & (TTsize - 1);

    if ((transpositionalTable[index].id == entry.id) && transpositionalTable[index].depth > entry.depth) {
        return;
    };

    transpositionalTable[index] = entry;

};

//MOVE ORDERING --------
bool ClassicChess::isCaptureMove(const Move& move) const
{
    return move.captured != NO_PIECE || move.type == EN_PASSANT_MOVE;
}

void ClassicChess::storeKillerMove(const Move& move, int depth) {
    if (depth < 0 || depth >= MAX_SEARCH_DEPTH) return;
    if (isCaptureMove(move)) return;

    if (killerValid[depth][0] && sameMove(move, killerMoves[depth][0])) return;

    killerMoves[depth][1] = killerMoves[depth][0];
    killerValid[depth][1] = killerValid[depth][0];

    killerMoves[depth][0] = move;
    killerValid[depth][0] = true;
}

void ClassicChess::clearKillerMoves() {
    killerValid = {};
}

// analyze which batch a move belongs in
ClassicChess::MoveBunch ClassicChess::analyzeMove(Move& move, const Move& TTmove, bool isTT, int depth) {


    if (isTT && sameMove(move, TTmove)) {
        return TT;
    }

    if (isCaptureMove(move)) {
        int capturedValue = piece_value(move.captured);
        int attackerValue = piece_value(move.moved);

        move.value = capturedValue * 5 - attackerValue;
        return CAPTURES;
    }

    if (depth >= 0 && depth < MAX_SEARCH_DEPTH) {
        if (killerValid[depth][0] && sameMove(move, killerMoves[depth][0])) {
            stats.killerHits++;
            move.value = 900000;
            return KM;
        }

        if (killerValid[depth][1] && sameMove(move, killerMoves[depth][1])) {
            stats.killerHits++;
            move.value = 800000;
            return KM;
        }
    }

    return QUIET;

}

// generate an array with movesets ordered by TT move, Captures, Killer Moves, Quiet Moves
std::array<ClassicChess::MoveSet, 4>
ClassicChess::GenerateOrderedLegalMoves(bool is_white, const Move& TTmove, bool isTT, int depth)
{
    std::array<MoveSet, 4> orderedLegalMoves;

    std::vector<Move> moves = generate_legal_moves(is_white);

    for (Move& move : moves)
    {
        MoveBunch category = analyzeMove(move, TTmove, isTT, depth);
        orderedLegalMoves[category].moves.push_back(move);
    }

    auto CompareByValue = [](const Move& a, const Move& b)
        {
            return a.value > b.value;
        };

    std::sort(
        orderedLegalMoves[CAPTURES].moves.begin(),
        orderedLegalMoves[CAPTURES].moves.end(),
        CompareByValue
    );

    return orderedLegalMoves;
}

// LOGS & BENCHMARK TESTING ------------

//clear ai search stats 
void ClassicChess::resetSearchStats() {
    stats = SearchStats{};
}

//print ai search stats
void ClassicChess::printSearchStats(int depth) {

    std::cout << "\n===== SEARCH STATS =====\n";
    std::cout << "Depth:          " << depth << '\n';
    std::cout << "Time:           " << stats.elapsedMs << " ms\n";
    std::cout << "Nodes:          " << stats.nodes << '\n';
    std::cout << "Leaf Nodes:     " << stats.leafNodes << '\n';
    std::cout << "Cutoffs:        " << stats.alphaBetaCutoffs << '\n';
    std::cout << "TT Hits:        " << stats.ttHits << '\n';
    std::cout << "TT Stores:      " << stats.ttStores << '\n';
    std::cout << "Killer Hits:    " << stats.killerHits << '\n';

    if (stats.elapsedMs > 0.0) {
        double nps =
            stats.nodes / (stats.elapsedMs / 1000.0);

        std::cout << "Nodes/sec:      "
            << static_cast<uint64_t>(nps)
            << '\n';
    }

    std::cout << "========================\n";
}
