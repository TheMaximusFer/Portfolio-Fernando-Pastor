package aima.core.environment.nqueens;

// Java estándar
import java.util.Set;
import java.util.HashSet;
import java.util.List;
import java.util.ArrayList;
import java.util.Random;
import aima.core.environment.nqueens.NQueensBoard;
import aima.core.environment.nqueens.NQueensFunctionFactory;
import aima.core.environment.nqueens.NQueensGoalTest;
import aima.core.environment.nqueens.AttackingPairsHeuristic;
import aima.core.environment.nqueens.NQueensFitnessFunction;
import aima.core.util.datastructure.XYLocation;
import aima.core.search.framework.GoalTest;
import aima.core.search.framework.Problem;
import aima.core.search.framework.SearchAgent;
import aima.core.search.local.HillClimbingSearch;
import aima.core.search.local.SimulatedAnnealingSearch;
import aima.core.search.local.Scheduler;
import aima.core.search.local.GeneticAlgorithm;
import aima.core.search.local.Individual;




public class NQueensLocal {

    static int _boardSize = 8;
    

    public static void main(String[] args) throws Exception {

        System.out.println("===== 2.1 HillClimbingSearch / Escalada =====");
        nQueensHillClimbingSearch_Statistics(10000);

        System.out.println("\n===== 2.2 Random-Restart Hill Climbing =====");
        nQueensRandomRestartHillClimbing();

        System.out.println("\n===== 2.3 SimulatedAnnealing / Enfriamiento Simulado (dos Schedulers) =====");
        nQueensSimulatedAnnealing_Statistics(1000);

        System.out.println("\n===== 2.4 GeneticAlgorithm / Algoritmos Genéticos =====");
        nQueensGeneticAlgorithmSearch();

        System.out.println("\n--- FIN ---");
    }



    //2.1
    public static void nQueensHillClimbingSearch_Statistics(int numExperiments) throws Exception {
        Set<String> vistos = new HashSet<>();  //hash para declarar los estados ya vistos.
        int exitos = 0;
        int fallos = 0;
        long pasosExitos = 0;
        long pasosFallos = 0;

        while (vistos.size() < numExperiments) {
            NQueensBoard board = randomNQueensBoard();
            String firma = sacarFirmaBoard(board);
            if (!vistos.add(firma)) continue; //evita repetidos.

            Problem problem = new Problem(board, NQueensFunctionFactory.getCActionsFunction(), 
                                        NQueensFunctionFactory.getResultFunction(), new NQueensGoalTest());
            HillClimbingSearch search = new HillClimbingSearch(new AttackingPairsHeuristic());
            SearchAgent agent = new SearchAgent(problem, search);

            // Pasos = número de acciones aplicadas
            int pasos = agent.getActions().size();

            boolean exito = search.getOutcome().toString().contentEquals("SOLUTION_FOUND");
            if (exito) {
                exitos++;
                pasosExitos += pasos;
            } else {
                fallos++;
                pasosFallos += pasos;
            }
        }

        double pctExitos = (double)exitos/numExperiments * 100;
        double pctFallos = (double)fallos/numExperiments * 100;
        double mediaExitos = (exitos == 0) ? 0.0 : ((double) pasosExitos / exitos);
        double mediaFallos = (fallos == 0) ? 0.0 : ((double) pasosFallos / fallos);
        double mediaTotal = ((double) (pasosExitos + pasosFallos)) / numExperiments;

        System.out.println("NQueens HillClimbing con " + numExperiments + " estados iniciales diferentes -->");
        System.out.printf("Fallos: %.2f%%%n", pctFallos);
        System.out.printf("Coste medio fallos: %.2f%n", mediaFallos);
        System.out.printf("Éxitos: %.2f%%%n", pctExitos);
        System.out.printf("Coste medio éxitos: %.2f%n", mediaExitos);
        System.out.println("Número de pasos medio: " + Math.round(mediaTotal));
    }

    //2.2
    public static void nQueensRandomRestartHillClimbing() throws Exception {
        int reinicios = 0;
        int pasosTotales = 0;
        boolean exito = false;

        while (!exito) {
            NQueensBoard board = randomNQueensBoard();
            Problem problem = new Problem(board, NQueensFunctionFactory.getCActionsFunction(),
                                        NQueensFunctionFactory.getResultFunction(), new NQueensGoalTest());
            HillClimbingSearch search = new HillClimbingSearch(new AttackingPairsHeuristic());
            SearchAgent agent = new SearchAgent(problem, search);

            //Suma los pasos de este intento.
            int pasos = agent.getActions().size();
            pasosTotales += pasos;
            
            exito = search.getOutcome().toString().contentEquals("SOLUTION_FOUND");
            if (exito) {
                double media_pasos = (double) pasosTotales/(reinicios + 1);
                exito = true;
                System.out.println("Numero de experimentos: " + (reinicios + 1));
                System.out.println("Número medio de pasos en conseguir el éxito: " + Math.round(media_pasos));
            } else {
                reinicios++;
            }
        }

        return;
    }

    //2.3
    public static void nQueensSimulatedAnnealing_Statistics(int numExperiments) throws Exception {
        //Configuración T alta (k grande, lambda pequeña).
        Scheduler schAlta = new Scheduler(20, 0.045, 100);
        sacarStatsSA(numExperiments, schAlta, "Parámetros Scheduler: Scheduler (20,0.045,100)");

        System.out.println();

        //Configuración T baja (k pequeña, lambda mayor).
        Scheduler schBaja = new Scheduler(5, 0.10, 100);
        sacarStatsSA(numExperiments, schBaja, "Parámetros Scheduler: Scheduler (5,0.10,100)");
    }

    private static void sacarStatsSA(int numExperiments, Scheduler scheduler, String etiqueta) throws Exception {
        Set<String> vistos = new HashSet<>();
        int exitos = 0;
        int fallos = 0;
        long pasosExitos = 0;
        long pasosFallos = 0;

        while (vistos.size() < numExperiments) {
            NQueensBoard board = randomNQueensBoard();
            String firma = sacarFirmaBoard(board);
            if (!vistos.add(firma)) continue; //evita repetidos.

            Problem problem = new Problem(board, NQueensFunctionFactory.getCActionsFunction(),
                                        NQueensFunctionFactory.getResultFunction(), new NQueensGoalTest());
            SimulatedAnnealingSearch search = new SimulatedAnnealingSearch(new AttackingPairsHeuristic(), scheduler);
            SearchAgent agent = new SearchAgent(problem, search);

            int pasos = agent.getActions().size();
            boolean exito = search.getOutcome().toString().contentEquals("SOLUTION_FOUND");

            if (exito) {
                exitos++;
                pasosExitos += pasos;
            } else {
                fallos++;
                pasosFallos += pasos;
            }
        }

        double pctExitos = (double) exitos/numExperiments * 100.0;
        double pctFallos = (double) fallos/numExperiments * 100.0;
        double mediaFallos = (fallos == 0) ? 0.0 : ((double) pasosFallos/fallos);
        double mediaExitos = (exitos == 0) ? 0.0 : ((double) pasosExitos/exitos);
        double mediaTotal = ((double) (pasosExitos + pasosFallos)) / numExperiments;

        System.out.println("NQueensDemo Simulated Annealing con " + numExperiments +
                        " estados iniciales diferentes -->");
        System.out.println(etiqueta);
        System.out.printf("Fallos: %.2f%%%n", pctFallos);
        System.out.printf("Coste medio fallos: %.2f%n", mediaFallos);
        System.out.printf("Éxitos: %.2f%%%n", pctExitos);
        System.out.printf("Coste medio éxitos: %.2f%n", mediaExitos);
        System.out.println("Número de pasos medio: " + Math.round(mediaTotal));

    }

    //2.4
    public static void nQueensGeneticAlgorithmSearch() throws Exception {
        // --- Parámetros a tunear ---
        final int tamPoblacion = 150;    //30–200
        final double pmut = 0.05;       //0.02–0.3
        final int maxGeneraciones = 670;

        sacarGenetic(tamPoblacion, pmut, maxGeneraciones);
    }

    private static void sacarGenetic(int tamPoblacion, double pmut, int maxGeneraciones){

        NQueensFitnessFunction FF = new NQueensFitnessFunction();

        //Población inicial aleatoria. Cada individuo es una lista de enteros (filas de las reinas en cada columna).
        Set<Individual<Integer>> poblacion = new HashSet<>(tamPoblacion);
        Random rnd = new Random();
        for (int i = 0; i < tamPoblacion; i++) {
            List<Integer> individuo = new ArrayList<>(_boardSize);
            for (int col = 0; col < _boardSize; col++) individuo.add(rnd.nextInt(_boardSize));
            poblacion.add(new Individual<>(individuo));
        }

        //Crea el algoritmo genético con los parámetros dados.
        GeneticAlgorithm<Integer> GA = new GeneticAlgorithm<>(_boardSize, FF.getFiniteAlphabetForBoardOfSize(_boardSize), pmut);

        // Fitness máximo teórico para N reinas = N*(N-1)/2.
        final double maxFit = _boardSize * (_boardSize - 1) / 2.0;


        // Ejecuta hasta maxGeneraciones o hasta alcanzar FF máximo
        long t0 = System.currentTimeMillis();
        Individual<Integer> mejorIndividuo = GA.geneticAlgorithm(poblacion, FF, FF, maxGeneraciones);
        long tf = System.currentTimeMillis() - t0;

        NQueensBoard mejorIndividuoBoard = FF.getBoardForIndividual(mejorIndividuo);
        double mejorIndividuoFit = FF.getValue(mejorIndividuo);
        boolean esObjetivo = FF.isGoalState(mejorIndividuo);
        int iteraciones = GA.getIterations();



        System.out.println("GeneticAlgorithm");
        System.out.println("Parámetros iniciales:    Población: " + tamPoblacion + ", Probabilidad mutación: " + pmut);
        System.out.println("Mejor individuo=");
        System.out.println(mejorIndividuoBoard); // imprime el tablero con Q y -

        System.out.println();
        System.out.println("Tamaño tablero  = " + _boardSize);
        System.out.println("Fitness         = " + mejorIndividuoFit);
        System.out.println("Es objetivo     = " + esObjetivo);
        System.out.println("Tamaño población= " + tamPoblacion);
        System.out.println("Iteraciones     = " + (iteraciones >= 0 ? iteraciones : maxGeneraciones));
        System.out.println("Tiempo          = " + tf + "ms.");
    }

    // ----- Auxiliares -----
    //Crea un tablero N-Reinas con una configuración aleatoria de reinas.
    private static NQueensBoard randomNQueensBoard(){
        NQueensBoard b = new NQueensBoard(_boardSize);

        //Coloca una reina por columna en fila aleatoria.
        for (int c = 0; c < _boardSize; c++) {
            b.addQueenAt(new XYLocation(c, (int)(Math.random()*_boardSize) ));
        }
        return b;
    }

    //Crea una cadena con la posición de las reinas en cada columna, para la firma en el hash
    private static String sacarFirmaBoard(NQueensBoard b) {
        StringBuilder sb = new StringBuilder();
        for (int c = 0; c < _boardSize; c++) {
            //Busca la fila de la reina en la columna c.
            int fila = -1;
            for (int f = 0; f < _boardSize; f++) {
                if (b.queenExistsAt(new XYLocation(c, f))){ 
                    fila = f; 
                    break; 
                }
            }
            sb.append(fila);
            if (c + 1 < _boardSize) sb.append(',');
        }
        return sb.toString();
    }

}
