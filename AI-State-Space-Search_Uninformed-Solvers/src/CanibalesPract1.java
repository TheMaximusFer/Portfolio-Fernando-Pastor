package aima.gui.demo.search;

import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import java.util.concurrent.*;

import aima.core.agent.Action;
import aima.core.environment.canibales.*;
import aima.core.search.framework.GraphSearch;
import aima.core.search.framework.TreeSearch;
import aima.core.search.framework.Problem;
import aima.core.search.framework.ResultFunction;
import aima.core.search.framework.Search;
import aima.core.search.framework.SearchAgent;
import aima.core.search.uninformed.BreadthFirstSearch;
import aima.core.search.uninformed.DepthFirstSearch;
import aima.core.search.uninformed.DepthLimitedSearch;
import aima.core.search.uninformed.IterativeDeepeningSearch;
import aima.core.search.uninformed.UniformCostSearch;


public class CanibalesPract1{

    private static final long TIMEOUT_S = 8; //segundos de tope por caso


    private static Search tipoSearch(String algoritmo){
        switch (algoritmo) {
            case "BFS-GRAPH":  return new BreadthFirstSearch(new GraphSearch());
            case "BFS-TREE":   return new BreadthFirstSearch(new TreeSearch());
            case "DFS-GRAPH":  return new DepthFirstSearch(new GraphSearch());
            case "DFS-TREE":   return new DepthFirstSearch(new TreeSearch());
            case "DLS-3":      return new DepthLimitedSearch(3);
            case "DLS-9":      return new DepthLimitedSearch(9);
            case "DLS-11":     return new DepthLimitedSearch(11);
            case "IDDFS":      return new IterativeDeepeningSearch();
            case "UCS-GRAPH":  return new UniformCostSearch(new GraphSearch());
            case "UCS-TREE":   return new UniformCostSearch(new TreeSearch());
            default: throw new IllegalArgumentException("Algoritmo desconocido: " + algoritmo);
        }
    }

    //Función para imprimir formato: resumen
    private static void imprimirResumenEnunciado(String alg, Properties p, long ms){
        System.out.println("Misioneros y caníbales " + alg + " -->");
        System.out.println("pathCost : " + p.getProperty("pathCost"));
        System.out.println("nodesExpanded : " + p.getProperty("nodesExpanded","0"));
        System.out.println("queueSize : " + p.getProperty("queueSize","0"));
        System.out.println("maxQueueSize : " + p.getProperty("maxQueueSize","0"));
        System.out.println("Tiempo:" + ms + "ms");
        System.out.println();
    }

    //Función para imprimir formato: repetición de un carácter n veces
    private static String rep(char ch, int n){
        if (n <= 0) return "";
        StringBuilder sb = new StringBuilder();
        for (int i=0;i<n;i++){
            if (i>0) sb.append(' ');
            sb.append(ch);
        }
        return sb.toString();
    }
    
    //Función para imprimir formato: estados
    private static String formatoDelState(CanibalesBoard s){
        String izda  = (rep('M', s.mIzda()) + (s.mIzda()>0 && s.cIzda()>0 ? " " : "") + rep('C', s.cIzda())).trim();
        String dcha  = (rep('M', s.mDcha()) + (s.mDcha()>0 && s.cDcha()>0 ? " " : "") + rep('C', s.cDcha())).trim();
        StringBuilder line = new StringBuilder();
        line.append("RIBERA-IZDA ");
        if (!izda.isEmpty()) line.append(izda).append(' ');
        if (s.boteIzda()) line.append("BOTE ");
        line.append("--RIO-- ");
        if (!s.boteIzda()) line.append("BOTE ");
        if (!dcha.isEmpty()) line.append(dcha).append(' ');
        line.append("RIBERA-DCHA");
        return line.toString();
    }

    //Función para imprimir formato: imprimir camino encontrado
    private static void imprimirCamino(List<Action> actions, Problem problem){
        ResultFunction rf = problem.getResultFunction();
        CanibalesBoard init = (CanibalesBoard) problem.getInitialState();

        //reconstruimos trayectoria
        List<CanibalesBoard> trayectoria = new ArrayList<>();
        trayectoria.add(init);
        CanibalesBoard cur = init;
        for (Action a : actions) {
            cur = (CanibalesBoard) rf.result(cur, a);
            trayectoria.add(cur);
        }

        System.out.println("SOLUCIÓN:");
        System.out.println("GOAL STATE");
        System.out.println(formatoDelState(trayectoria.get(trayectoria.size()-1)));
        System.out.println();
        System.out.println("CAMINO ENCONTRADO");
        System.out.println("ESTADO INICIAL\t" + formatoDelState(trayectoria.get(0)));

        for (int i = 0; i < actions.size(); i++){
            Action a = actions.get(i);
            CanibalesBoard next = trayectoria.get(i+1);
            System.out.print("\t" + a.toString());
            System.out.println("\t" + formatoDelState(next));
        }
        System.out.println();
    }

    //Ejecución de una búsqueda y salida con el formato del enunciado
    private static void mcSearch(String algoritmo, CanibalesBoard estado_inicial){
        Problem problem = new Problem(estado_inicial,CanibalesFunctionFactory.getActionsFunction(),
                                     CanibalesFunctionFactory.getResultFunction(), new CanibalesGoalTest());
        
                                      Search search = tipoSearch(algoritmo);

        ExecutorService hilo = Executors.newSingleThreadExecutor();
        long t0 = System.nanoTime();
        Future<SearchAgent> fut = hilo.submit(() -> new SearchAgent(problem, search));
        try {
            SearchAgent agent = fut.get(TIMEOUT_S, TimeUnit.SECONDS);

            long t1 = System.nanoTime();

            Properties p = agent.getInstrumentation();


            imprimirResumenEnunciado(algoritmo, p, Math.round((t1 - t0) / 1e6));
            if (agent.getActions().get(0).toString().equals("Action[name==CutOff]")){
                //DLS con profundidad insuficiente
                System.out.println("Search Outcome=CUTOFF\n");
            }else{
                imprimirCamino(agent.getActions(), problem);
            }

        } catch (TimeoutException te){
            fut.cancel(true);
            long ms = Math.round((System.nanoTime() - t0) / 1e6);

            System.out.println("Misioneros y caníbales " + algoritmo + " -->");
            System.out.println("pathCost : (1)");
            System.out.println("nodesExpanded : -");
            System.out.println("queueSize : -");
            System.out.println("maxQueueSize : -");
            System.out.println("Tiempo:" + ms + "ms\n");
        } catch (ExecutionException | InterruptedException e){
            fut.cancel(true);
            long ms = Math.round((System.nanoTime() - t0) / 1e6);
            System.out.println("Misioneros y caníbales " + algoritmo + " -->");
            System.out.println("pathCost : (1)");
            System.out.println("nodesExpanded : -");
            System.out.println("queueSize : -");
            System.out.println("maxQueueSize : -");
            System.out.println("Tiempo:" + ms + "ms\n");
            Thread.currentThread().interrupt();
        } finally {
            hilo.shutdownNow();
        }
    }

    public static void main(String[] args){
        String[] algoritmos = {"BFS-GRAPH","BFS-TREE","DFS-GRAPH","DFS-TREE","DLS-9","DLS-3","IDDFS","UCS-GRAPH","UCS-TREE"};
        CanibalesBoard estadoInicial = new CanibalesBoard(3, 3, true);   //3M, 3C y bote a la izquierda

        for (String alg : algoritmos) mcSearch(alg, estadoInicial);

    }
}
