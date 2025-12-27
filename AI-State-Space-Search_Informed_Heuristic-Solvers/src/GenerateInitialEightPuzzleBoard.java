package aima.gui.demo.search;

import java.util.HashSet;
//import java.util.Iterator;
import java.util.Random;
import java.util.Set;

import aima.core.environment.eightpuzzle.EightPuzzleBoard;

/**
 * @author JAB
 * 
 */

public class GenerateInitialEightPuzzleBoard {
	
	
	public static EightPuzzleBoard randomIni(){
		Random r = new Random();
		EightPuzzleBoard board = new EightPuzzleBoard(new int[] { 0, 1, 2, 3,
				4, 5, 6, 7, 8 });
		for (int i = 0; i < 50; i++) {
			int th = r.nextInt(4);
			if (th == 0) {
				board.moveGapUp();
			}
			if (th == 1) {
				board.moveGapDown();
			}
			if (th == 2) {
				board.moveGapLeft();
			}
			if (th == 3) {
				board.moveGapRight();
			}
		}
		return board;
	}
	
	// //devuelve un estado aleatorio de profundidad depth desde 
	// // {0, 1, 2, 3, 4, 5, 6, 7, 8} 
	// public static EightPuzzleBoard random(int depth) {
	// 	 Set<Object> statesGenerated = new HashSet<Object>();
	// 	 //System.out.println("Generating random state");

	// 	Random r = new Random();
	// 	EightPuzzleBoard board = new EightPuzzleBoard(new int[] { 0, 1, 2, 3,
	// 			4, 5, 6, 7, 8 });
	// 	EightPuzzleBoard newBoard = new EightPuzzleBoard(board);
		
	// 	statesGenerated.add(new EightPuzzleBoard(board));
	// 	int pasos = 1;
	// 	while (pasos<=depth) {

	// 		newBoard = new EightPuzzleBoard(board);
	// 		int th = r.nextInt(4);
	// 		if (th == 0) {
	// 			newBoard.moveGapUp();
	// 			if   (!statesGenerated.contains(newBoard)){
	// 				 statesGenerated.add(new EightPuzzleBoard(newBoard));
	// 				 board.moveGapUp();
	// 				 pasos++;
	// 			}
	// 		}
	// 		if (th == 1) {
	// 			newBoard.moveGapDown();
	// 			if   (!statesGenerated.contains(newBoard)){
	// 				 statesGenerated.add(new EightPuzzleBoard(newBoard));
	// 				 board.moveGapDown();
	// 				 pasos++;
	// 			}
	// 		}
	// 		if (th == 2) {
	// 			newBoard.moveGapLeft();
	// 			if   (!statesGenerated.contains(newBoard)){
	// 				 statesGenerated.add(new EightPuzzleBoard(newBoard));
	// 				 board.moveGapLeft();
	// 				 pasos++;
	// 			}			
	// 		}
	// 		if (th == 3) {
	// 			newBoard.moveGapRight();
	// 			if   (!statesGenerated.contains(newBoard)){
	// 				 statesGenerated.add(new EightPuzzleBoard(newBoard));
	// 				 board.moveGapRight();
	// 				 pasos++;
	// 			}				
	// 		}
			
			
	// 	}
		

	// 	return(board);

	// }
	public static EightPuzzleBoard random(int depth) {
		Random r = new Random();
		int maxPasosIntentados = 100;  	//Bajo porque máx. movimientos=4; si no encuentra en 100 intentos, probablemente atascado.
		int maxIntentosGlobales = 10000;//Previene reintentos infinitos para profundidades muy altas.
		int IntentosGlobales = 0;

		while (IntentosGlobales < maxIntentosGlobales){
			IntentosGlobales++;
			Set<Object> statesGenerated = new HashSet<Object>();
			EightPuzzleBoard board = new EightPuzzleBoard(new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8 });
			statesGenerated.add(new EightPuzzleBoard(board));
			int pasos = 1;
			boolean atascado = false;
			
			while (pasos <= depth) {
				int PasosIntentados = 0;
				boolean extendido = false;
				while (PasosIntentados < maxPasosIntentados) {
					PasosIntentados++;
					EightPuzzleBoard newBoard = new EightPuzzleBoard(board);
					int th = r.nextInt(4);
					if (th == 0) {
						newBoard.moveGapUp();
					} else if (th == 1) {
						newBoard.moveGapDown();
					} else if (th == 2) {
						newBoard.moveGapLeft();
					} else if (th == 3) {
						newBoard.moveGapRight();
					}
					if (!statesGenerated.contains(newBoard)) {
						statesGenerated.add(new EightPuzzleBoard(newBoard));
						board = newBoard;  //Actualiza el tablero al nuevo estado.
						pasos++;
						extendido = true;
						break;
					}
				}
				if (!extendido) {
					atascado = true;
					break;  //Atascado en este paso, reintenta toda la generación.
				}
			}
			if (!atascado) {
				return board;  //Éxito.
			}
			//Si atascado, el bucle reinicia desde el estado objetivo.
		}
		
		//Si se excede maxIntentosGlobales (raro), usa randomIni().
		System.err.println("Falló al generar tablero en profundidad " + depth + " después de " + maxIntentosGlobales + " intentos. Usando randomIni() como alternativa.");
		return randomIni();
	}
	


	
	//devuelve un estado aleatorio de profundidad depth desde 
	//initial State. Ojo, no hay garantia de que no haya una solucion mas corta.
	public static EightPuzzleBoard random(int depth, EightPuzzleBoard initialState){
		 Set<Object> statesGenerated = new HashSet<Object>();

		Random r = new Random();
		EightPuzzleBoard board = new EightPuzzleBoard(initialState);
		EightPuzzleBoard newBoard = new EightPuzzleBoard(board);
		
		statesGenerated.add(new EightPuzzleBoard(board));
		int pasos = 1;
		int limit = 100000; //OJO podria no encontrar solucion de esa profundidad. Ponemos un limite de iteraciones
		int cont = 1;
		while (pasos<=depth && cont < limit) {
			newBoard = new EightPuzzleBoard(board);
			int th = r.nextInt(4);
			if (th == 0) {
				newBoard.moveGapUp();
				if   (!statesGenerated.contains(newBoard)){
					 statesGenerated.add(new EightPuzzleBoard(newBoard));
					 board.moveGapUp();
					 pasos++;
				}
			}
			if (th == 1) {
				newBoard.moveGapDown();
				if   (!statesGenerated.contains(newBoard)){
					 statesGenerated.add(new EightPuzzleBoard(newBoard));
					 board.moveGapDown();
					 pasos++;
				}
			}
			if (th == 2) {
				newBoard.moveGapLeft();
				if   (!statesGenerated.contains(newBoard)){
					 statesGenerated.add(new EightPuzzleBoard(newBoard));
					 board.moveGapLeft();
					 pasos++;
				}			
			}
			if (th == 3) {
				newBoard.moveGapRight();
				if   (!statesGenerated.contains(newBoard)){
					 statesGenerated.add(new EightPuzzleBoard(newBoard));
					 board.moveGapRight();
					 pasos++;
				}				
			}
			
		cont++;	
		}	

		return(board);
	}

}