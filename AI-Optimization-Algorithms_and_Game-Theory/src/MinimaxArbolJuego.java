package aima.core.search.adversarial;

public class MinimaxArbolJuego{

    public static final class Resultado{
        public final double valor;
        public final int nodosVisitados;
        public Resultado(double valor, int nodosVisitados){
            this.valor = valor;
            this.nodosVisitados = nodosVisitados;
        }
    }

    public Resultado ejecutar(ArboldeJuego<Double> raiz){
        int[] contador = new int[]{0};
        double v = minimax(raiz, true, contador);
        return new Resultado(v, contador[0]);
    }

    private double minimax(ArboldeJuego<Double> n, boolean esRaiz, int[] contador){
        if(!esRaiz) contador[0]++;

        if(n.esTerminal()){
            n.setVisitado();
            return n.getValor();
        }

        double mejor;
        if (n.esMax()){
            mejor = Double.NEGATIVE_INFINITY;
            for (ArboldeJuego<Double> h : n.getHijos()){
                double v = minimax(h, false, contador);
                if (v > mejor) mejor = v;
            }
        } else { //Min.
            mejor = Double.POSITIVE_INFINITY;
            for (ArboldeJuego<Double> h : n.getHijos()){
                double v = minimax(h, false, contador);
                if (v < mejor) mejor = v;
            }
        }

        //Guardamos el valor en el nodo interno para que se muestre en printArbolExplorado().
        n.setValor(mejor);
        return mejor;
    }
}
