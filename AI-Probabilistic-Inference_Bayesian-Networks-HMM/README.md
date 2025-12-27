# 👻 Probabilistic Inference: Ghost Tracking with Bayes Nets & HMMs

## 📌 Project Overview
This project implements a **Probabilistic Reasoning System** to locate invisible moving targets ("ghosts") in a grid environment using noisy sensor data.

Unlike standard search algorithms (A*), the agent does not have full state visibility. Instead, it maintains a **Belief Distribution** over the ghosts' positions, updating probabilities over time using **Hidden Markov Models (HMMs)** and **Bayesian Networks**.

## 🧠 Key Algorithms Implemented

### 1. Bayesian Inference & Factor Operations (`factorOperations.py`)
Implemented the core algebra for manipulating probability tables (Factors) in a Bayes Net:
* **Join Operation:** Combines multiple factors $P(A) \times P(B|A)$ into a joint distribution.
* **Variable Elimination:** Optimized inference by summing out irrelevant variables from the joint distribution. This reduces exponential complexity to polynomial time in many network structures.

### 2. Bayesian Network Construction (`inference.py`)
* **Dynamic Bayes Net (DBN):** Constructed a network representing the game state, linking ghost positions at time $t$ to time $t+1$ and sensor readings to current positions.
* **Observation Model:** Implemented $P(E|X)$, calculating the probability of obtaining a noisy distance reading given a true ghost position.

### 3. Greedy Busters Agent (`bustersAgents.py`)
* **Decision Making under Uncertainty:** The agent utilizes the computed posterior distribution $P(GhostPos | Observations)$ to choose actions.
* It targets the cell with the highest probability (Maximum A Posteriori estimate) or minimizes expected distance to the target.

## 📂 Repository Structure

### 🛠 Source Code
This project is built upon the **UC Berkeley CS188 AI** framework. The files provided here contain the custom inference logic:

| File Name | Description |
| :--- | :--- |
| `factorOperations.py` | Implements **Variable Elimination** (Join/Eliminate) for efficient probabilistic query processing. |
| `inference.py` | Defines the **Bayes Net structure**, observation models, and belief update logic. |
| `bustersAgents.py` | The autonomous agent that uses inference modules to hunt ghosts efficiently. |

> **Note:** These files are part of a larger ecosystem (game engine, graphics). They require the full CS188 Tracking project structure to execute.

## ⚙️ Mathematical Foundations

The core of the tracking logic relies on the **Forward Algorithm** for HMMs:

$$P(X_{t+1} | e_{1:t+1}) = \alpha \cdot P(e_{t+1} | X_{t+1}) \sum_{x_t} P(X_{t+1} | x_t) P(x_t | e_{1:t})$$

Where:
* **Time Elapse:** $P(X_{t+1} | x_t)$ models the ghost's stochastic movement.
* **Observation Update:** $P(e_{t+1} | X_{t+1})$ corrects the belief based on noisy sensor data.
* **Normalization:** $\alpha$ ensures probabilities sum to 1.

## 🚀 Usage Context

To run this agent within the Berkeley AI Pacman environment (if the full engine is present):

```bash
# Run the inference agent with a fixed random seed
python busters.py -p GreedyBustersAgent -l smallHunt

# Run with visible probability distributions
python busters.py -s -k 1 -g InferenceGhost