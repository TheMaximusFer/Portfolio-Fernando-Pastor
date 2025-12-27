package aima.core.environment.canibales;

import java.util.HashSet;
import java.util.Set;

import aima.core.agent.Action;
import aima.core.agent.impl.DynamicAction;
import aima.core.search.framework.ActionsFunction;
import aima.core.search.framework.ResultFunction;
import aima.core.search.framework.StepCostFunction;

public class CanibalesFunctionFactory {

    public static final Action M1C  = new DynamicAction("M1C");
    public static final Action M1M  = new DynamicAction("M1M");
    public static final Action M2M  = new DynamicAction("M2M");
    public static final Action M2C  = new DynamicAction("M2C");
    public static final Action M1M1C= new DynamicAction("M1M1C");



    /*
    * Realiza un movimiento, moviendo m misioneros y c caníbales, desde el lado donde está el bote hacia el otro lado.
    * Si el bote está a la izquierda, resta en la izquierda, si no, está a la derecha, entonces suma en la izquierda.
    * Cambia la posición del bote.
    */
    private static CanibalesBoard mover(CanibalesBoard s, int m, int c){
        if(s.boteIzda() == true){
            return new CanibalesBoard(s.mIzda() - m, s.cIzda() - c, false);
        }else{
            return new CanibalesBoard(s.mIzda() + m, s.cIzda() + c, true);
        }
    }

    /** Estado legal: conteos válidos y, en cada orilla, si hay misioneros (>0) entonces m ≥ c. */
    private static boolean legal(CanibalesBoard s) {
        
        //Si hay números negativos o se exceden los totales en la izquierda, no es legal.
        if (s.mIzda() < 0 || s.cIzda() < 0 || s.mIzda() > CanibalesBoard.TOTAL_M || s.cIzda() > CanibalesBoard.TOTAL_C){
                return false;
        }

        //Si hay números negativos o se exceden los totales en la derecha, no es legal.
        if (s.mDcha() < 0 || s.cDcha() < 0) return false;

        //Si hay más caníbales que misioneros en alguna orilla, no es legal.
        boolean okIzda  = (s.mIzda() == 0) || (s.mIzda() >= s.cIzda());
        boolean okDcha = (s.mDcha() == 0)  || (s.mDcha() >= s.cDcha());

        //En cualquier otro caso, es legal.
        return okIzda && okDcha;
    }



    //Devolver lista de posibles acciones al expandir un estado.
    public static class MCActionsFunction implements ActionsFunction {
        @Override
        public Set<Action> actions(Object stateObj) {
            CanibalesBoard s = (CanibalesBoard) stateObj;
            Set<Action> out = new HashSet<>();

            CanibalesBoard t;

            t = mover(s, 0, 1);  //M1C
            if (legal(t)) out.add(M1C);

            t = mover(s, 1, 0);  //M1M
            if (legal(t)) out.add(M1M);

            t = mover(s, 2, 0);  //M2M
            if (legal(t)) out.add(M2M);

            t = mover(s, 0, 2);  //M2C
            if (legal(t)) out.add(M2C);

            t = mover(s, 1, 1);  //M1M1C
            if (legal(t)) out.add(M1M1C);

            return out;
        }
    }

    //Resultado de aplicar una acción.
    public static class MCResultFunction implements ResultFunction {
        @Override
        public Object result(Object stateObj, Action action) {
            CanibalesBoard s = (CanibalesBoard) stateObj;

            if      (action.equals(M1C))   return mover(s, 0, 1);
            else if (action.equals(M1M))   return mover(s, 1, 0);
            else if (action.equals(M2M))   return mover(s, 2, 0);
            else if (action.equals(M2C))   return mover(s, 0, 2);
            else if (action.equals(M1M1C)) return mover(s, 1, 1);
            else throw new IllegalArgumentException("Acción no válida: " + action);
        }
    }

    //Coste uniforme, cada uno cuesta 1
    public static class MCSimpleStepCost implements StepCostFunction {
        @Override
        public double c(Object s, Action a, Object s2){return 1.0;}
    }

    // Factories
    public static ActionsFunction  getActionsFunction()   { return new MCActionsFunction(); }
    public static ResultFunction   getResultFunction()    { return new MCResultFunction(); }
    public static StepCostFunction getStepCostFunction()  { return new MCSimpleStepCost(); }


}
