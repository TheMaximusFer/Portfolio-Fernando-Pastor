package aima.gui.demo.search;

import aima.core.agent.Action;
import aima.core.environment.eightpuzzle.EightPuzzleBoard;
import aima.core.environment.eightpuzzle.EightPuzzleFunctionFactory;
import aima.core.environment.eightpuzzle.EightPuzzleGoalTest2;
import aima.gui.demo.search.GenerateInitialEightPuzzleBoard;
import aima.core.environment.eightpuzzle.ManhattanHeuristicFunction2;
import aima.core.environment.eightpuzzle.MisplacedTilleHeuristicFunction2;
import aima.core.search.framework.GraphSearch;
import aima.core.search.framework.Problem;
import aima.core.search.framework.Search;
import aima.core.search.framework.SearchAgent;
import aima.core.search.informed.AStarSearch;
import aima.core.search.uninformed.BreadthFirstSearch;
import aima.core.search.uninformed.IterativeDeepeningSearch;
import aima.core.util.math.Biseccion;
import java.util.Properties;


public class EightPuzzlePract2{

    private static final int[] DEPTHS = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
    private static final int NUM_EXPERIMENTOS = 100;
    private static final int MAX_DEPTH_IDS = 10; //IDS solo hasta d=10, como en la tabla ejemplo.

    private static Biseccion biseccion = new Biseccion();
    private static EightPuzzleGoalTest2 goalTest = new EightPuzzleGoalTest2();
    private static MisplacedTilleHeuristicFunction2 h1 = new MisplacedTilleHeuristicFunction2();
    private static ManhattanHeuristicFunction2 h2 = new ManhattanHeuristicFunction2();

    

    public static void main(String[] args) {

        cabecera();

        //Configurar goal test.
        goalTest.setGoalState(new EightPuzzleBoard(new int[]{0, 1, 2, 3, 4, 5, 6, 7, 8}));
    
        //Configurar heurísticas.
        h1.setGoalTest(goalTest.getGoalState());
        h2.setGoalTest(goalTest.getGoalState());
    
        //Generar 100 estados iniciales para esta profundidad.
        for (int d : DEPTHS){  
            double[] totalNodes = new double[4]; //BFS, IDS, A*h1, A*h2
            for (int exp = 0; exp < NUM_EXPERIMENTOS; exp++){
                EightPuzzleBoard inicial = GenerateInitialEightPuzzleBoard.random(d);

                //Ejecutar cada algoritmo y sumar nodos generados.
                totalNodes[0] += eightPuzzleSearch("BFS", inicial);
                if (d <= MAX_DEPTH_IDS) {
                    totalNodes[1] += eightPuzzleSearch("IDS", inicial);
                } else {
                    totalNodes[1] += 0; //Para media, pero se marcará como ---
                }
                totalNodes[2] += eightPuzzleSearch("A*-H1", inicial);
                totalNodes[3] += eightPuzzleSearch("A*-H2", inicial);
            }

            //Calcular medias.
            double[] avgNodes = new double[4];
            for (int i = 0; i < 4; i++) {
                avgNodes[i] = totalNodes[i] / NUM_EXPERIMENTOS;
            }

            //Calcular b*.
            biseccion.setDepth(d);
            double bBFS = calcularBestrella(avgNodes[0]);
            double bIDS = (d <= MAX_DEPTH_IDS) ? calcularBestrella(avgNodes[1]) : -1;
            double bH1 = calcularBestrella(avgNodes[2]);
            double bH2 = calcularBestrella(avgNodes[3]);


            imprimirFila(d, avgNodes, bBFS, bIDS, bH1, bH2);
        }

        System.out.println("------------------------------------------------------------------------------------------");
    }

    

    private static Search tipoSearch(String algoritmo) {
        switch (algoritmo) {
            case "BFS":         return new BreadthFirstSearch(new GraphSearch());
            case "IDS":         return new IterativeDeepeningSearch();
            case "A*-H1":       return new AStarSearch(new GraphSearch(), h1);
            case "A*-H2":       return new AStarSearch(new GraphSearch(), h2);
            default: throw new IllegalArgumentException();
        }
    }



    private static double eightPuzzleSearch(String algoritmo, EightPuzzleBoard estado_inicial) {
        
        try {
            Problem problem = new Problem(estado_inicial, EightPuzzleFunctionFactory.getActionsFunction(),
                            EightPuzzleFunctionFactory.getResultFunction(), goalTest);
            
            Search search = tipoSearch(algoritmo);

            SearchAgent agent = new SearchAgent(problem, search);
            Properties properties = agent.getInstrumentation();
            int generated = Integer.parseInt(properties.getProperty("nodesGenerated", "0"));

            return generated;
        
        }catch (Exception e) {
            System.gc();
            return 0;
        }
    }



    private static double calcularBestrella(double N) {
        if (N <= 1) return 0.0;
        biseccion.setGeneratedNodes((int) N);
        return biseccion.metodoDeBiseccion(1.0001, 10.0, 0.001);
    }



    private static void cabecera() {
        System.out.println("-----------------------------------------------------------------------------------------");
        System.out.println("||    ||     Nodos Generados                   ||                  b*                  ||");
        System.out.println("-----------------------------------------------------------------------------------------");
        System.out.println("||   d||    BFS  |    IDS  | A*h(1)  | A*h(2)  ||    BFS  |    IDS  | A*h(1)  | A*h(2) ||");
        System.out.println("-----------------------------------------------------------------------------------------");
        System.out.println("-----------------------------------------------------------------------------------------");
                            
    }



    private static void imprimirFila(int d, double[] avgNodes, double bBFS, double bIDS, double bH1, double bH2) {
        String idsNodes = (d > MAX_DEPTH_IDS) ? "---" : String.format("%6.0f", avgNodes[1]);
        String idsB = (d > MAX_DEPTH_IDS) ? "---" : String.format("%6.2f", bIDS);

        System.out.printf("|| %3d|| %7.0f | %7s | %7.0f | %7.0f || %7.2f | %7s | %7.2f | %6.2f ||\n",
                           d, avgNodes[0], idsNodes, avgNodes[2], avgNodes[3], bBFS, idsB, bH1, bH2);
    }
}
