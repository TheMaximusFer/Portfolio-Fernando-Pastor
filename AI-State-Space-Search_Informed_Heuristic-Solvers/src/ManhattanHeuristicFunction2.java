package aima.core.environment.eightpuzzle;

import aima.core.search.framework.HeuristicFunction;
import aima.core.util.datastructure.XYLocation;

public class ManhattanHeuristicFunction2 implements HeuristicFunction {

    private EightPuzzleBoard goal;

    public ManhattanHeuristicFunction2(){}

    public ManhattanHeuristicFunction2(EightPuzzleBoard goal){ 
        this.goal = goal; 
    }

    public void setGoalTest(EightPuzzleBoard goal){
        this.goal = goal;
    }


	public double h(Object state){
        EightPuzzleGoalTest2 goalState = new EightPuzzleGoalTest2();
		EightPuzzleBoard goal = goalState.getGoalState();
        EightPuzzleBoard board = (EightPuzzleBoard) state;
        int retVal = 0;
        for (int i = 1; i < 9; i++){
            XYLocation loc = board.getLocationOf(i);
            XYLocation goalLoc = goal.getLocationOf(i);
            retVal += manhattanDistance(loc, goalLoc);
        }
        return retVal;
    }

    private int manhattanDistance(XYLocation loc1, XYLocation loc2) {
        return Math.abs(loc1.getXCoOrdinate() - loc2.getXCoOrdinate()) +
               Math.abs(loc1.getYCoOrdinate() - loc2.getYCoOrdinate());
    }
}