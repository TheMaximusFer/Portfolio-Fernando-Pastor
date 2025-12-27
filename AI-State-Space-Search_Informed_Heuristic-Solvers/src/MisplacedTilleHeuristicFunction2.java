package aima.core.environment.eightpuzzle;

import aima.core.search.framework.HeuristicFunction;
import aima.core.util.datastructure.XYLocation;


public class MisplacedTilleHeuristicFunction2 implements HeuristicFunction {

    private EightPuzzleBoard goal;

    public MisplacedTilleHeuristicFunction2(){}

    public MisplacedTilleHeuristicFunction2 (EightPuzzleBoard goal){ 
        this.goal = goal; 
    }

    public void setGoalTest(EightPuzzleBoard goal){
        this.goal = goal;
    }

    public double h(Object state) {
        EightPuzzleBoard board = (EightPuzzleBoard) state;
        return getNumberOfMisplacedTiles(board);
    }

    private int getNumberOfMisplacedTiles(EightPuzzleBoard board) {
        EightPuzzleGoalTest2 goalState = new EightPuzzleGoalTest2();
        int numberOfMisplacedTiles = 0;
        for (int i = 1; i <= 8; i++) {
            XYLocation locActual = board.getLocationOf(i);
            XYLocation locGoal = goal.getLocationOf(i);
            if (!locActual.equals(locGoal)) {
                numberOfMisplacedTiles++;
            }
        }
        return numberOfMisplacedTiles;
    }
}

