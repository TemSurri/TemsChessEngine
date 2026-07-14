# Tem's C++ Chess Engine

<p align="center">
  <img src="https://github.com/user-attachments/assets/622801ad-dc28-4e3b-9eb6-09718a58125a" width="31%">
  <img src="https://github.com/user-attachments/assets/ba6b46ae-a0ca-4ceb-9138-6431c150a071" width="31%">
  <img src="https://github.com/user-attachments/assets/b75c9f11-84bf-459e-bf87-cf11fd4bd63c" width="31%">
</p>

A chess engine built completely from scratch in **C++** with a custom **OpenGL** renderer and GUI.

The engine supports full legal move generation, Minimax with Alpha-Beta pruning, Iterative Deepening, Transposition Tables, Zobrist Hashing, Killer Move Heuristics, and a bitboard board representation. Every optimization was implemented and benchmarked individually to understand how modern chess engines search efficiently.

Current performance:

* **5M+ nodes/second**
* **~1 ms** for positions that took my original Python engine **3-7 seconds**
* **20-60× faster** than where this project originally began

---

## Features

* Full legal move generation
* Castling, en passant, and promotion
* Make / Undo move system
* Minimax with Alpha-Beta pruning
* Iterative Deepening
* Zobrist Hashing
* Transposition Tables
* TT, Capture, and Killer move ordering
* Bitboard board representation
* Custom OpenGL renderer (GLFW, GLAD, shaders, VBOs, VAOs, EBOs, texture atlases)
* Search profiling and benchmarking

---

## Benchmarks (Release)

| Depth | Typical Time |
| ----: | -----------: |
|     5 | ~0.01-0.03 s |
|     6 | ~0.03-0.15 s |
|     7 |  ~0.15-0.8 s |
|     8 |   ~0.5-1.5 s |

---

## Running

Clone the repository, build in **Release**, and run:

```bash
git clone <repo>

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

./build/Chess
```

---

## Development Journey

This project started as a rewrite of my high-school Python chess engine. Instead of continuing to optimize slow code, I rebuilt the engine from scratch and treated it as an opportunity to learn how chess engines actually work.

The project evolved through several major rewrites—from an object-based board, to arrays, and finally to bitboards. Along the way I implemented Alpha-Beta pruning, Iterative Deepening, Transposition Tables, Zobrist Hashing, move ordering, and several search heuristics, benchmarking each optimization as I went.

One of the biggest lessons was that performance isn't just about writing faster code—it's about avoiding unnecessary work. Most of the speedup came from making the engine search smarter, not simply making the CPU evaluate positions faster.

---

## Challenges

The biggest challenge wasn't writing Minimax—it was getting all of the chess rules correct while keeping the engine fast.

Special moves like castling, en passant, promotion, and legal move filtering all had edge cases that interacted with the search and undo system. Rewriting the board representation multiple times also meant rebuilding large parts of the engine while preserving correctness.

By far the most valuable part of the project was learning how seemingly small architectural decisions compound into massive performance differences when an engine evaluates millions of positions.
