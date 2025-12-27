package aima.core.search.adversarial;

public class AlfaBetaArbolJuego{

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
        double v = alphabeta(raiz, true, contador, Double.NEGATIVE_INFINITY, Double.POSITIVE_INFINITY);
        return new Resultado(v, contador[0]);
    }

    private double alphabeta(ArboldeJuego<Double> n, boolean esRaiz, int[] contador, double alpha, double beta) {
        if(!esRaiz) contador[0]++;

        if(n.esTerminal()){
            n.setVisitado();
            return n.getValor();
        }

        if(n.esMax()){
            double value = Double.NEGATIVE_INFINITY;
            for (ArboldeJuego<Double> h : n.getHijos()) {
                double v = alphabeta(h, false, contador, alpha, beta);
                if (v > value) value = v;
                if (value > alpha) alpha = value;
                if (alpha >= beta) {
                    //Poda.
                    break;
                }
            }
            n.setValor(value);
            return value;
        }else { //Min.
            double value = Double.POSITIVE_INFINITY;
            for (ArboldeJuego<Double> h : n.getHijos()) {
                double v = alphabeta(h, false, contador, alpha, beta);
                if (v < value) value = v;
                if (value < beta) beta = value;
                if (alpha >= beta) {
                    //Poda.
                    break;
                }
            }
            n.setValor(value);
            return value;
        }
    }
}
