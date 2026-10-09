# Logic in CS - Assignment 1

This submission contains a complete C++17 solution for Part A and a Lean 4 proof file for Part B. The program represents one propositional formula as a rooted parse tree, then derives every required result from that tree.

## What the program does

- Converts a fully parenthesized infix formula to prefix notation.
- Rebuilds a rooted parse tree from the prefix expression.
- Prints recursive and iterative inorder, preorder, and postorder traversals.
- Computes tree height recursively and iteratively.
- Evaluates the formula bottom-up and prints its complete truth table.
- Reads DIMACS CNF benchmarks (including SAT 2002 inputs) and translates them into the assignment notation before running the same analysis.

The allowed connectives are `~` (not), `+` (or), `*` (and), and `>` (implication). Atoms may be letters, digits, and underscores. Binary expressions must be fully parenthesized; `~P` and `(~P)` are both accepted.

## Project layout

| Path | Purpose |
| --- | --- |
| `src/main.cpp` | Command-line interface and reporting. |
| `include/logic_tree.hpp` | Parse tree, parsers, traversals, evaluator, truth-table and DIMACS logic. |
| `lean/Assignment1.lean` | Lean 4 proofs of Examples 1.13-1.18. |
| `docs/analysis.html` | Required HTML design/results/complexity report. |
| `Doxyfile` | Doxygen configuration for the C++ source. |

## Build and run

### With CMake

```powershell
cmake -S . -B build
cmake --build build --config Release
.\build\Release\logic_tool.exe --formula "((p*q)>r)"
```

On toolchains that do not use configuration subfolders, use `./build/logic_tool` (or `./build/logic_tool.exe`) instead.

### With a direct compiler command

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/main.cpp -o logic_tool
.\logic_tool.exe --formula "((p*q)>r)"
```

### DIMACS CNF benchmark

Download a `.cnf` file from the SAT 2002 benchmark collection and run:

```powershell
.\build\Release\logic_tool.exe --cnf path\to\benchmark.cnf
```

Small formulas are best for displaying a truth table. A benchmark with `v` atoms produces `2^v` rows, so large SAT inputs are useful for testing parser/conversion behavior but impractical to tabulate exhaustively.

## Sample result

For `((p*q)>r)`, the program produces prefix `> * p q r`, reconstructs the same infix expression with both inorder implementations, reports height `3`, and prints all eight valuations of `p`, `q`, and `r`.

## Documentation

The source uses Doxygen comments. If Doxygen is installed, generate browsable API documentation with:

```powershell
doxygen Doxyfile
```

Open `docs/analysis.html` in a browser for the written algorithm analysis required in Task 6.

## Lean Part B

Open `lean/Assignment1.lean` in VS Code with the Lean 4 extension, or add it to a Lean project. Each theorem is named after the corresponding textbook example and has a tactic proof that Lean verifies.

