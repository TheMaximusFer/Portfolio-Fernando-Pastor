public class Main{
    public static void main(String[] args){

        System.out.println("Ejecutando programa: Suma de dos números");
        Programa suma = new ProgramaSuma();
        System.out.println("Listado:");
        suma.listar();
        System.out.println("Ejecución:");
        suma.ejecutar();
        System.out.println("\n------------------------------\n");

        System.out.println("Ejecutando programa: Cuenta atrás");
        Programa cuentaAtras = new ProgramaCuentaAtras();
        System.out.println("Listado:");
        cuentaAtras.listar();
        System.out.println("Ejecución:");
        cuentaAtras.ejecutar();
        System.out.println("\n------------------------------\n");


        System.out.println("Ejecutando programa: Factorial");
        Programa factorial = new ProgramaFactorial();
        System.out.println("Listado:");
        factorial.listar();
        System.out.println("Ejecución:");
        factorial.ejecutar();
        System.out.println("\n------------------------------\n");
    }
}
