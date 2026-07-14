# Tem's C++ Chess Engine

<p align="center">
  <img src="https://github.com/user-attachments/assets/622801ad-dc28-4e3b-9eb6-09718a58125a" width="31%">
  <img src="https://github.com/user-attachments/assets/ba6b46ae-a0ca-4ceb-9138-6431c150a071" width="31%">
  <img src="https://github.com/user-attachments/assets/b75c9f11-84bf-459e-bf87-cf11fd4bd63c" width="31%">
</p>

A chess engine built completely from scratch in **C++** with a custom **OpenGL** interface.

The engine supports full legal move generation, Alpha-Beta pruning, Iterative Deepening, Zobrist Hashing, Transposition Tables, Killer Move Heuristics, and a bitboard board representation. It currently searches between **2.4M and 3.8M positions per second**, making it roughly **20–60× faster** than the project it originally started from.

Every optimization was implemented and benchmarked individually to better understand how modern chess engines search efficiently, rather than simply making the CPU work harder.

---

## Performance

Current Release benchmarks:

| Depth | Typical Time |
|------:|-------------:|
| 5 | ~0.01–0.04 s |
| 6 | ~0.05–0.20 s |
| 7 | ~0.2–1.3 s |
| 8 | ~0.7–2.0 s |

Current search throughput:

- **2.4M–3.8M nodes/sec**
- **20–60× faster** than where the project originally began
- **~38× faster** than my optimized array-based implementation

---

## Engine

Current features include:

- Full legal move generation
- Castling, en passant, and promotion
- Make / Undo move system
- Alpha-Beta pruning
- Iterative Deepening
- Zobrist Hashing
- Transposition Tables
- TT move ordering
- Capture move ordering
- Killer move heuristic
- Bitboard board representation
- Search profiling and benchmarking

The GUI is also written from scratch using modern **OpenGL**, including custom rendering, texture atlases, shaders, vertex buffers, and mouse interaction. The engine and renderer are completely separate, allowing the chess engine to be used independently of the interface.

---

## Running

Clone the repository, build it in **Release**, and run the executable.

```bash
git clone <repo>

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

./build/Chess
```

---

## Development Journey

This project originally started as a rewrite of my high-school Python chess engine.

The original engine worked, but every board lookup required searching through collections of piece objects. Once Minimax started evaluating hundreds of thousands of positions every move, that approach quickly became the bottleneck.

The first rewrite replaced the object-based board with direct board indexing, making common lookups constant time. From there, the project slowly evolved into an exercise in understanding chess engine optimization.

Rather than adding everything at once, I implemented each optimization individually and benchmarked the engine after every change.

The search gradually gained:

- Alpha-Beta pruning
- Transposition Tables
- Zobrist Hashing
- Iterative Deepening
- Move Ordering
- Killer Moves
- Bitboards

One of the biggest performance improvements turned out to be compiling in **Release** instead of **Debug**.

Without changing any search code, performance increased from roughly **100k nodes/sec** to almost **800k nodes/sec**.

The final major rewrite replaced the array-based board representation with bitboards, allowing many board operations to become simple bitwise instructions. The search algorithm itself stayed almost entirely the same—the goal was simply to make every position cheaper to evaluate.

That ultimately pushed the engine to roughly **2.4M–3.8M nodes/sec**.

Sometimes the best optimization isn't inventing a new algorithm.

It's making the existing one ridiculously fast.

---

## What's Next?

There are still plenty of improvements worth exploring:

- Magic Bitboards
- Incremental Evaluation
- Stronger Evaluation Function
- Endgame Tablebases
- Multithreading
- Additional search optimizations

For now, I'm focused on polishing the OpenGL interface before moving on to the next generation of engine improvements.
