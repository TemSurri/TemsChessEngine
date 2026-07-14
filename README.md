# Tem's C++ Chess Engine

<p align="center">
  <img src="https://github.com/user-attachments/assets/ba6b46ae-a0ca-4ceb-9138-6431c150a071" width="31%">
  <img src="https://github.com/user-attachments/assets/b75c9f11-84bf-459e-bf87-cf11fd4bd63c" width="31%">
  <img src="https://github.com/user-attachments/assets/622801ad-dc28-4e3b-9eb6-09718a58125a" width="31%">
</p>

A chess engine built completely from scratch in **C++** with a custom **OpenGL GUI**.

Originally this project was just a rewrite of my high-school Python chess engine. The old engine worked... if you didn't mind waiting **4–7 seconds** every move.

Instead of continuing to optimize it, I decided to rebuild the entire thing from scratch.

Somewhere along the way, it became an obsession with making numbers go up.

---

## Features

- ♟️ Full legal move generation
- 🏰 Castling, en passant, and promotion
- 🔄 Make / Undo move system
- 🌳 Minimax with Alpha-Beta pruning
- ⚡ Iterative Deepening
- 🧠 Transposition Tables with Zobrist Hashing
- 🎯 TT, Capture, and Killer move ordering
- 💾 Bitboard board representation
- 🖥️ Custom OpenGL GUI
- 📊 Search profiling and benchmarking

---

## Performance Journey

The biggest improvements came from changing **how** the engine searched, not how fast the CPU was.

I implemented and benchmarked each optimization individually:

- Alpha-Beta pruning
- Transposition Tables
- Zobrist Hashing
- Iterative Deepening
- Move Ordering
- Killer Moves
- Bitboards

One of the funniest improvements wasn't even algorithmic.

Compiling in **Release** instead of **Debug** increased performance from roughly:

```
~100k nodes/sec
```

to

```
~800k nodes/sec
```

without changing a single line of search code.

Turns out the compiler had been carrying me the whole time.

---

## Bitboards

The final major rewrite replaced my array-based board with **bitboards**.

Instead of repeatedly searching arrays and objects, each piece type is stored inside a `uint64_t`, allowing many board operations to become simple bitwise instructions.

The search algorithm stayed exactly the same.

The goal wasn't to search differently—it was simply to make every position cheaper to evaluate.

That pushed performance to roughly:

**2.4M–3.8M nodes/sec**

or about **20–60× faster** than where this project originally started.

---

## Benchmarks (Release)

| Depth | Typical Time |
|------:|-------------:|
| 5 | ~0.01–0.04 s |
| 6 | ~0.05–0.20 s |
| 7 | ~0.2–1.3 s |
| 8 | ~0.7–2.0 s |

Current search speed:

- **2.4M–3.8M nodes/sec**
- **20–60× faster** than my original Python engine
- **~38× faster** than my optimized array-based C++ implementation

---

## What's Next?

I'm currently polishing the OpenGL interface before moving on to stronger engine improvements.

Some ideas I'd like to explore:

- Magic Bitboards
- Incremental Evaluation
- Better Evaluation Function
- Endgame Tablebases
- Multithreading
- Additional search optimizations

---

## Demo

Gameplay video coming soon.

The plan is to have one of my low/mid-rated friends play against it so the engine can finally bully a real human instead of just my CPU.

---

This definitely isn't Stockfish.

But compared to the Python chess engine I built in high school, it might as well be.
