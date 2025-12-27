# 🧠 Heuristic Search & A* Optimization

## 📌 Project Overview
This project focuses on **Informed Search Strategies**, specifically implementing and analyzing the **A* (A-Star)** algorithm. It compares the performance of heuristic-based solvers against uninformed baselines (BFS, IDS) in the context of the **8-Puzzle Problem**.

The core objective is to demonstrate how domain-specific knowledge (heuristics) drastically reduces the search space. Two admissible heuristics are implemented and benchmarked:
1.  **Misplaced Tiles ($h_1$):** Number of tiles not in their goal position.
2.  **Manhattan Distance ($h_2$):** Sum of the vertical and horizontal distances of the tiles from their goal positions.

## 📂 Repository Structure

### 📄 Documentation
* [**Read the Technical Report (English)**](./Report_Informed-Search_(English).pdf) – Analysis of Effective Branching Factor ($b^*$) and algorithmic efficiency.
* *[Ver Memoria Original (Spanish)](./Report_Informed-Search_(Español_Original).pdf) – Original academic submission.*

### 🛠 Source Code & Integration
This project extends the **[AIMA-Java (v1.8.1)](https://github.com/aimacode/aima-java/releases/tag/aima3e-v1.8.1)** framework. To run the benchmarks, place the source files into the following package structure:

| File Name | Target Package Path (inside `aima-core/src/main/java`) | Description |
| :--- | :--- | :--- |
| **Heuristics & Environment** | | |
| `ManhattanHeuristicFunction2.java` | `aima.core.environment.eightpuzzle` | Implementation of the Manhattan distance ($h_2$) heuristic. |
| `MisplacedTilleHeuristicFunction2.java` | `aima.core.environment.eightpuzzle` | Implementation of the Misplaced Tiles ($h_1$) heuristic. |
| `EightPuzzleGoalTest2.java` | `aima.core.environment.eightpuzzle` | Custom goal state validator. |
| **Execution & Main** | | |
| `EightPuzzlePract2.java` | `aima.gui.demo.search` | **Main Benchmark Runner**. Executes BFS, IDS, and A* comparisons. |
| `GenerateInitialEightPuzzleBoard.java` | `aima.gui.demo.search` | Helper to generate random puzzle states with solvability checks. |
| **Core Framework Mods** | | |
| `NodeExpander.java` | `aima.core.search.framework` | *Modified* to track nodes generated/expanded metrics accurately. |
| `IterativeDeepeningSearch.java` | `aima.core.search.uninformed` | *Modified* to support comparative metrics against A*. |

---

## 🚀 How to Run

1.  Clone the **AIMA-Java** repository (tag `aima3e-v1.8.1`).
2.  Integrate the files listed above into their respective packages.
3.  Compile the project using your IDE or build tool.
4.  Run the main class: `aima.gui.demo.search.EightPuzzlePract2`

## 📊 Performance Analysis
The project analyzes the **Effective Branching Factor ($b^*$)** and **Time Complexity** across 100 random instances for depths $d=2$ to $d=24$.

### Key Findings:
* **A* vs. Blind Search:** A* provides an exponential reduction in generated nodes compared to IDS and BFS.
* **Heuristic Dominance:** The **Manhattan Distance ($h_2$)** heuristic consistently outperforms Misplaced Tiles ($h_1$).
    * $h_2$ is a tighter lower bound to the actual cost $h^*(n)$, pruning the search tree more effectively.
* **Optimality:** Both implemented heuristics are admissible, guaranteeing that A* finds the optimal solution (shortest path).