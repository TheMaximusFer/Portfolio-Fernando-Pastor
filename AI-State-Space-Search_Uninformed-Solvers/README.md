# 🔍 Uninformed Search Algorithms & Problem Solving

## 📌 Project Overview
This project explores **State Space Search** using fundamental uninformed algorithms (**BFS, DFS, IDS, UCS**). It leverages the **AIMA-Java framework** to solve classic AI problems, analyzing performance metrics such as time complexity, space complexity, and node expansion.

The implementation focuses on two primary domains:
1.  **The 8-Puzzle Problem:** A standard sliding tile puzzle to test search efficiency.
2.  **Missionaries and Cannibals:** A constraint satisfaction transport problem modeled from scratch.

## 📂 Repository Structure

### 📄 Documentation
* [**Read the Technical Report (English)**](./Report_Uninformed-Search_(English).pdf) – Detailed algorithmic analysis and performance benchmarks.
* *[Ver Memoria Original (Spanish)](./Report_Uninformed-Search_(Español_Original).pdf) – Original academic submission.*

### 🛠 Source Code & Integration
This project extends the **[AIMA-Java (v1.8.1)](https://github.com/aimacode/aima-java/releases/tag/aima3e-v1.8.1)** library. To compile and run the code, you must place the provided source files into the following package directory structure:

| File Name | Target Package Path (inside `aima-core/src/main/java`) | Description |
| :--- | :--- | :--- |
| **Problem Modeling** | | |
| `CanibalesBoard.java` | `aima.core.environment.canibales` | State representation for Missionaries & Cannibals. |
| `CanibalesFunctionFactory.java` | `aima.core.environment.canibales` | Defines actions (Move 1M, 1C, etc.) and transition logic. |
| `CanibalesGoalTest.java` | `aima.core.environment.canibales` | Validates if the goal state (all on right bank) is reached. |
| **Search Framework Core** | | |
| `NodeExpander.java` | `aima.core.search.framework` | *Modified* to add custom node generation metrics. |
| `IterativeDeepeningSearch.java` | `aima.core.search.uninformed` | *Modified* implementation to support metric tracking. |
| **Execution & Main** | | |
| `EightPuzzlePract1.java` | `aima.gui.demo.search` | **Main entry point** for running 8-Puzzle benchmarks. |
| `CanibalesPract1.java` | `aima.gui.demo.search` | **Main entry point** for running Missionaries & Cannibals benchmarks. |

---

## 🚀 How to Run

1.  Clone the **AIMA-Java** repository (tag `aima3e-v1.8.1`).
2.  Integrate the files listed above into their respective source packages, overwriting `NodeExpander.java` and `IterativeDeepeningSearch.java` if necessary to enable custom metrics.
3.  Compile the project using your preferred Java IDE (IntelliJ/Eclipse) or Maven.
4.  Run the main classes:
    * **For 8-Puzzle:** Execute `aima.gui.demo.search.EightPuzzlePract1`
    * **For Missionaries:** Execute `aima.gui.demo.search.CanibalesPract1`

## 📊 Key Results
* **Breadth-First Search (BFS):** Guaranteed optimality for unweighted graphs but high memory consumption.
* **Depth-First Search (DFS):** Lower memory footprint but fails to find the optimal solution in cyclic state spaces without loop checking.
* **Iterative Deepening Search (IDS):** Balanced approach combining BFS optimality with DFS space efficiency.