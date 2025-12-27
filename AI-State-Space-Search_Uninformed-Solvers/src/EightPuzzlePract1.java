package aima.gui.demo.search;

import java.util.Iterator;
import java.util.List;
import java.util.Properties;
import java.util.concurrent.*;
import aima.core.agent.Action;
import aima.core.environment.eightpuzzle.EightPuzzleBoard;
import aima.core.environment.eightpuzzle.EightPuzzleFunctionFactory;
import aima.core.environment.eightpuzzle.EightPuzzleGoalTest;
import aima.core.environment.eightpuzzle.ManhattanHeuristicFunction;
import aima.core.environment.eightpuzzle.MisplacedTilleHeuristicFunction;
import aima.core.search.framework.GraphSearch;
import aima.core.search.framework.TreeSearch;
import aima.core.search.framework.Problem;
import aima.core.search.framework.Search;
import aima.core.search.framework.SearchAgent;
import aima.core.search.framework.ResultFunction;
import aima.core.search.local.SimulatedAnnealingSearch;
import aima.core.search.uninformed.BreadthFirstSearch;
import aima.core.search.uninformed.DepthFirstSearch;
import aima.core.search.uninformed.DepthLimitedSearch;
import aima.core.search.uninformed.IterativeDeepeningSearch;
import aima.core.search.uninformed.UniformCostSearch;

/**
 * @author Fernando Pastor Peralta - 897113
 **/


public class EightPuzzlePract1 {
	static EightPuzzleBoard boardWithThreeMoveSolution = new EightPuzzleBoard(
			new int[] { 1, 2, 5, 3, 4, 0, 6, 7, 8 });;

	static EightPuzzleBoard random1 = new EightPuzzleBoard(new int[] { 1, 4, 2,
			7, 5, 8, 3, 0, 6 });

	static EightPuzzleBoard extreme = new EightPuzzleBoard(new int[] { 0, 8, 7,
			6, 5, 4, 3, 2, 1 });

    private static int pasos_sol = 0;

    private static final long TIMEOUT_S = 8; //segundos de tope por caso


    public static void executeActions(List<Action> actions, Problem problem) {
    Object state = problem.getInitialState();
    ResultFunction rf = problem.getResultFunction();

    System.out.println("INITIAL STATE");
    System.out.println(state);
    for (Action a : actions) {
        System.out.println(a.toString());
        state = rf.result(state, a);
        System.out.println(state);
        System.out.println("- - -");
    }
}




    private static Search tipoSearch(String algoritmo){
        switch(algoritmo){
            case "BFS-GRAPH":  return new BreadthFirstSearch(new GraphSearch());
            case "BFS-TREE":   return new BreadthFirstSearch(new TreeSearch());
            case "DFS-GRAPH":  return new DepthFirstSearch(new GraphSearch());
            case "DFS-TREE":   return new DepthFirstSearch(new TreeSearch());
            case "DLS-9":      return new DepthLimitedSearch(9);
            case "DLS-3":      return new DepthLimitedSearch(3);
            case "IDDFS":      return new IterativeDeepeningSearch();
            case "UCS-GRAPH":  return new UniformCostSearch(new GraphSearch());
            case "UCS-TREE":   return new UniformCostSearch(new TreeSearch());
            default:throw new IllegalArgumentException();
        }
    }



    private static void cabecera(){
        System.out.printf("%15s|%12s|%10s|%10s|%10s|%10s|%10s|%10s|\n", 
                          "Problema", "Profundidad", "Expand", "Q.Size", "MaxQS", "Tiempo(ms)", "Generados", "Ritmo");    
    }



    private static void imprimirResultado(String algoritmoYinicial, String profundidad, String expand, String queueSize, 
                                     String maxQueueSize, double tiempo, int generated, double ritmo){
        
        System.out.printf("%15s|%12d|%10s|%10s|%10s|%10.3f|%10d|%10.1f|\n", 
                          algoritmoYinicial, (int)Float.parseFloat(profundidad), expand, queueSize, maxQueueSize, tiempo, generated, ritmo);

    }



    private static void eightPuzzleSearch(String algoritmo, EightPuzzleBoard estado_inicial){

        String algoritmoYinicial = algoritmo + "-" +Integer.toString(pasos_sol);
        
        Problem problem = new Problem(estado_inicial, EightPuzzleFunctionFactory.getActionsFunction(), 
                                      EightPuzzleFunctionFactory.getResultFunction(), 
                                      new EightPuzzleGoalTest());

        ExecutorService hilo = Executors.newSingleThreadExecutor();

        Search search = tipoSearch(algoritmo);
        
        long t0 = System.nanoTime();
        Future<SearchAgent> fut = hilo.submit(() -> new SearchAgent(problem, search)); //fut manejará la tarea en el hilo
        try{
            SearchAgent agent = fut.get(TIMEOUT_S, TimeUnit.SECONDS); //espera máx TIMEOUT_S a que SercAgent termine, si no TimeoutException

            long t1 = System.nanoTime();

            Properties properties = agent.getInstrumentation();


            int generated = Integer.parseInt(properties.getProperty("nodesGenerated","0"));
            double tiempo_ms = (t1 - t0) / 1e6;
            double ritmo = generated/(tiempo_ms/1e3);

            imprimirResultado(algoritmoYinicial, properties.getProperty("pathCost") , properties.getProperty("nodesExpanded", "0"), 
                            properties.getProperty("queueSize","0"), properties.getProperty("maxQueueSize", "0"), 
                            tiempo_ms, generated, ritmo);
            
        }catch (TimeoutException | InterruptedException e){ //temporizador superado
            fut.cancel(true); //interrumpimos la búsqueda
            System.out.printf("%15s|         ---|       ---|       ---|       ---|       (1)|       ---|       ---|\n", algoritmoYinicial);
            System.gc();
            return;
        }catch (ExecutionException e){
            fut.cancel(true); //interrumpimos la búsqueda
            System.out.printf("%15s|         ---|       ---|       ---|       ---|       (2)|       ---|       ---|\n", algoritmoYinicial);
            System.gc();
            return;
        }catch (OutOfMemoryError e){ //heap superado
            fut.cancel(true); //interrumpimos la búsqueda
            System.out.printf("%15s|         ---|       ---|       ---|       ---|       (2)|       ---|       ---|\n", algoritmoYinicial);
            System.gc();
            return;
        }finally{
            hilo.shutdownNow();//cerramos el hilo
        }
    }



	public static void main(String[] args){
        String[] algoritmos = {"BFS-GRAPH", "BFS-TREE", "DFS-GRAPH", "DFS-TREE", "DLS-9", "DLS-3", "IDDFS", "UCS-GRAPH", "UCS-TREE"};

		cabecera();
        pasos_sol = 3;
        for(int i = 0; i < algoritmos.length; i++) eightPuzzleSearch(algoritmos[i], boardWithThreeMoveSolution);
        pasos_sol = 9;
        for(int i = 0; i < algoritmos.length; i++) eightPuzzleSearch(algoritmos[i], random1);
        pasos_sol = 30;
        for(int i = 0; i < algoritmos.length; i++) eightPuzzleSearch(algoritmos[i], extreme);
	}



}

