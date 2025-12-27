# 🧬 Optimization Algorithms & Game Theory

## 📌 Project Overview
This project explores **Local Search Optimization** and **Adversarial Search** in multi-agent environments. Moving beyond standard pathfinding, this implementation focuses on finding optimal configurations in vast state spaces and computing winning strategies in zero-sum games.

The project is divided into two core modules:
1.  **Constraint Optimization (N-Queens):** Comparison of **Hill Climbing**, **Simulated Annealing**, and **Genetic Algorithms** to solve the 8-Queens problem.
2.  **Game Theory (Adversarial Search):** Implementation of **Minimax** and **Alpha-Beta Pruning** on a custom Game Tree structure to demonstrate decision-making efficiency.

## 📂 Repository Structure

### 📄 Documentation
* [**Read the Technical Report (English)**](./Report_Local-Search_Games_(English).pdf) – Performance analysis of Genetic Algorithms and Alpha-Beta pruning efficiency.
* *[Ver Memoria Original (Spanish)](./Report_Local-Search_Games_(Español_Original).pdf) – Original academic submission.*

### 🛠 Source Code & Integration
This project extends the **[AIMA-Java (v1.8.1)](https://github.com/aimacode/aima-java/releases/tag/aima3e-v1.8.1)** framework. To execute the algorithms, integrate the provided source files into the following package structure:

| File Name | Target Package Path (inside `aima-core/src/main/java`) | Description |
| :--- | :--- | :--- |
| **Optimization (Local Search)** | | |
| `NQueensLocal.java` | `aima.core.environment.nqueens` | **Main Runner** for N-Queens. Implements Hill Climbing, Simulated Annealing, and Genetic Algorithms. |
| **Adversarial Search (Games)** | | |
| `MinimaxArbolJuego.java` | `aima.core.search.adversarial` | Implementation of the standard Minimax algorithm (DFS strategy). |
| `AlfaBetaArbolJuego.java` | `aima.core.search.adversarial` | Optimized implementation using Alpha-Beta Pruning to reduce search space. |
| `EjemplosArbolJuego.java` | `aima.core.search.adversarial` | **Main Runner** for Game Theory. Generates game trees and compares Minimax vs. Alpha-Beta. |

---

## 🚀 How to Run

1.  Clone the **AIMA-Java** repository (tag `aima3e-v1.8.1`).
2.  Place the files listed above into their respective directories.
3.  Compile the project using your IDE or build tool.

### To Run Optimization Benchmarks:
Execute the `NQueensLocal` class. This will run a comparative analysis of:
* **Hill Climbing:** (Steepest Ascent & Random Restart).
* **Simulated Annealing:** Using a custom scheduler.
* **Genetic Algorithm:** With specific population sizes and mutation probabilities.

```bash
java aima.core.environment.nqueens.NQueensLocal