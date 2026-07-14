# Tem C++ Chess Engin

<img width="1038" height="766" alt="Screenshot 2026-07-14 174930" src="https://github.com/user-attachments/assets/ba6b46ae-a0ca-4ceb-9138-6431c150a071" />
<img width="1110" height="705" alt="Screenshot 2026-07-14 174858" src="https://github.com/user-attachments/assets/b75c9f11-84bf-459e-bf87-cf11fd4bd63c" />
<img width="1129" height="826" alt="Screenshot 2026-07-14 175309" src="https://github.com/user-attachments/assets/622801ad-dc28-4e3b-9eb6-09718a58125a" />

A chess engine written completely from scratch in C++.

This project started as an attempt to rewrite my original Python chess engine, which worked... but only if you had the patience to wait **4–7 seconds** every time the AI wanted to think.

Instead of continuously adding features to a slow foundation, I decided to throw everything away and rebuild it properly.

Somewhere along the way, this turned into an obsession with making numbers go up.

---

# Rewriting the Engine

The original Python engine represented the board as a list of piece objects. Every time it needed to answer a simple question—*"Is there a piece on this square?"*—it had to iterate through every piece until it found one.

That worked perfectly fine for playing chess.

It didn't work very well when Minimax started evaluating **hundreds of thousands of positions** every move.

For the rewrite, I wanted the board itself to answer those questions instantly.

The new engine stores:

* `board[8][8]` for constant-time square lookup
* `whitePieces`
* `blackPieces`

Instead of repeatedly searching through every piece, the engine can directly index the board in **O(1)** time.

This simple architectural change completely transformed the project. The AI that once needed **4–7 seconds** to make relatively shallow decisions suddenly became responsive enough that deeper searches were actually worth pursuing.

Once I saw those improvements, chasing performance became the fun part.

---

# Chasing Bigger Numbers

With the engine architecture in a good place, I shifted my focus to the search algorithm.

One optimization quickly turned into another.

The engine gradually gained:

* Alpha-Beta Pruning
* Transposition Tables with Zobrist Hashing
* Iterative Deepening
* TT Move Ordering
* Capture Move Ordering
* Killer Move Heuristic

I implemented each of these one at a time, benchmarking the engine after every addition.

Before Alpha-Beta pruning, a **depth 5** search typically took **10–15 seconds**. **Depth 6** often took several minutes, and I honestly wasn't patient enough to find out how long depth 7 would take.

After implementing the complete search stack, the results looked very different:

* **Depth 5:** ~1–50 ms
* **Depth 7:** ~3–5 seconds

What's interesting is that none of these optimizations actually made the CPU process positions faster.

Instead, they made the engine **search smarter**.

Alpha-Beta pruning cuts away branches that can no longer influence the final result. Transposition Tables cache previously evaluated positions, avoiding repeated work. Iterative Deepening and move ordering help the engine discover strong candidate moves earlier, allowing even more branches to be pruned.

The amount of work being done dropped dramatically, even though each individual position still cost roughly the same to evaluate.

Then I accidentally discovered one of the biggest performance improvements of the entire project...

Compiling in **Release** instead of **Debug**. (lol)

Without changing a single search algorithm, performance jumped from roughly:

**~100k nodes/sec**

to nearly

**800k nodes/sec**

An almost **8× improvement**... thanks to checking the correct build configuration.

Turns out the compiler had been doing a lot of the heavy lifting all along.

---

# Bitboards

By this point, I was happy with the search algorithm itself.

The next bottleneck wasn't *how* the engine searched—it was simply **how fast each position could be processed**.

So I rewrote the board representation one final time using **bitboards**.

Instead of storing the board with arrays and object pointers, a bitboard represents piece locations inside a single `uint64_t`. This allows many common chess operations to be performed using fast bitwise instructions instead of loops and pointer lookups.

The search algorithm stayed exactly the same.

The only goal was to make the existing engine process positions faster.

That rewrite pushed performance to roughly:

**2.4M–3.8M nodes/sec**

Depending on the position, that's roughly a **20×–60× improvement** compared to where this project originally began.

Sometimes the best optimization isn't inventing a new algorithm.

It's making the old one ridiculously fast.

---

# Current Status

The engine currently supports:

* Full legal move generation
* Make / Undo move system
* Minimax
* Alpha-Beta Pruning
* Iterative Deepening
* Zobrist Hashing
* Transposition Tables
* TT Move Ordering
* Capture Move Ordering
* Killer Move Heuristic
* Bitboard board representation
* Search profiling and benchmarking

### Current Release Benchmarks

| Depth |                  Typical Time |
| ----: | ----------------------------: |
|     5 |                    ~0.1–0.4 s |
|     6 |                    ~0.2-0.1 s |
|     7 |                    ~0.2–1.3 s |
|     8 | ~0.7–2 s (position dependent) |

Current search throughput:

* **2.4M–3.8M nodes/second**
* **20×–60× faster** than where the project originally began
* **~38× faster** than my optimized array-based Release implementation

---

# Demo

I'll upload a short video here once the OpenGL interface is finished.

The plan is to have one of my low/mid-rated friends play against the engine so it can finally bully a real human instead of just my CPU.

*(Coming soon...)*

---

# What's Next?

For now, just a GUI.

The next step is building an **OpenGL interface** for the engine. Besides making the project much nicer to interact with, it will also serve as practice before I begin writing my own game engine from scratch.

Could this engine be stronger? Absolutely.

There are still plenty of ideas worth exploring, magic bitboards, incremental evaluation, stronger evaluation functions, endgame tablebases, multithreading, and many more search optimizations, but I'm satisfied with where the project is today.

This aint so stockfish, but compared to my python chess engine i made in high-school, it might as well be.

