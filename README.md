# Chess

This project is a rewrite of my original chess engine.

The old engine was extremely slow and could barely handle a Minimax opponent because almost every operation required unnecessary iteration and inefficient board lookups. The design worked for basic gameplay, but once AI search was introduced you would have to wait a solid 4-7 seconds before the AI could move.

## First Optimization

The first version of the engine stored pieces in a python list and relied heavily on iteration to determine board state.
So in this new c++ rewrite I introduced:

- whitePieces vector 
- blackPieces vector
- board[8][8] array storing pointers to pieces

Instead of iterating through every piece to determine whether a square is occupied, the engine can now directly index the board array in constant time.

RESULT
- O(1) move validation, rather than having to iterate over the entire list O(n).
- minimax searches are significiantly faster, almost instant moves.

## Second Optimization (todo)

The current version is already much better and should be good enough for basic Minimax + Alpha-Beta pruning.

However, I eventually want to push the engine further and make the board representation more efficient. The next optimization step would be moving toward bitboards, which use integers to represent the board instead of larger object-based structures.

This is not required for the engine to work, but it would make the engine faster, cleaner for advanced search, and closer to how high-performance cutting edge chess engines are actually designed.

### Predicted Improvements

- faster board operations
- lower memory usage
- faster AI search
- deeper search depth
- more scalable engine architecture

## Tech

- C++
- CMake
- STL
- Planned: OpenGL (for the GUI later after im done with the optimizations)
