package aima.core.search.adversarial;

import java.util.ArrayList;

public class EjemplosArbolJuego{

    public static void main(String[] args){
        ArboldeJuego<Double> t1 = generateTree2();
 
        MinimaxArbolJuego mm = new MinimaxArbolJuego();
        MinimaxArbolJuego.Resultado r1 = mm.ejecutar(t1);
        System.out.println("Valor con MINIMAX: " + r1.valor);
        t1.printArbolExplorado();
        System.out.println("Nodos visitados: " + r1.nodosVisitados);
        System.out.println("-------------------------------");

        ArboldeJuego<Double> t2 = generateTree2();

        AlfaBetaArbolJuego ab = new AlfaBetaArbolJuego();
        AlfaBetaArbolJuego.Resultado r2 = ab.ejecutar(t2);
        System.out.println("Valor con poda Alfa Beta: " + r2.valor);
        t2.printArbolExplorado();
        System.out.println("Nodos visitados: " + r2.nodosVisitados);
        System.out.println("-------------------------------");
    }

    
	public static  ArboldeJuego<Double>  generateTree2(){
		ArrayList<ArboldeJuego<Double>> sublist1 = 
			new ArrayList<ArboldeJuego<Double>>();
			sublist1.add(new ArboldeJuego<Double>(3.0,true));  //MAX, terminal 3
			sublist1.add(new ArboldeJuego<Double>(12.0,true)); //MAX, terminal 12
			sublist1.add(new ArboldeJuego<Double>(8.0,true));  //MAX, terminal 8
		ArboldeJuego<Double> subtree1 = 	// arbol MIN del que cuelga 3,12,8
			new ArboldeJuego<Double>(Double.POSITIVE_INFINITY,false,sublist1); //MIN, valor Intege.MIN_VALUE
		
		ArrayList<ArboldeJuego<Double>> sublist2 = 
			new ArrayList<ArboldeJuego<Double>>();
			sublist2.add(new ArboldeJuego<Double>(2.0,true));
			sublist2.add(new ArboldeJuego<Double>(4.0,true));
			sublist2.add(new ArboldeJuego<Double>(6.0,true));
		ArboldeJuego<Double> subtree2 =  // arbol MIN del que cuelga 2,4,6
			new ArboldeJuego<Double>(Double.POSITIVE_INFINITY,false,sublist2);
				
		ArrayList<ArboldeJuego<Double>> sublist3 = 
			new ArrayList<ArboldeJuego<Double>>();
			sublist3.add(new ArboldeJuego<Double>(14.0,true));
			sublist3.add(new ArboldeJuego<Double>(5.0,true));
			sublist3.add(new ArboldeJuego<Double>(2.0,true));
		ArboldeJuego<Double> subtree3 =  // arbol MIN del que cuelga 14,5,2
				   new ArboldeJuego<Double>(Double.POSITIVE_INFINITY,false,sublist3);
		
		ArrayList<ArboldeJuego<Double>> topTree =  //Top arrayList wiht subtrees
				new ArrayList<ArboldeJuego<Double>>();
			topTree.add(subtree1);
			topTree.add(subtree2);
			topTree.add(subtree3);
		ArboldeJuego<Double> tree= 
				   new ArboldeJuego<Double>(Double.NEGATIVE_INFINITY,true,topTree);
		return tree;
	}
}
