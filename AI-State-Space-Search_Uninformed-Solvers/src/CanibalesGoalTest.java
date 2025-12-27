package aima.core.environment.canibales;

import aima.core.search.framework.GoalTest;

public class CanibalesGoalTest implements GoalTest {
    @Override
    public boolean isGoalState(Object state) {
        CanibalesBoard s = (CanibalesBoard) state;
        //Meta: todos en la derecha (la posición del bote no importa)
        return s.mIzda() == 0 && s.cIzda() == 0;
    }
}
