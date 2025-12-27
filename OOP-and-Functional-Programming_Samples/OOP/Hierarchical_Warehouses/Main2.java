public class Main2 {
    public static void main(String[] args) {
        // ====================================================
        // 1. Prueba de productos estándar y contenedor de carga estándar
        // ====================================================
        // Se crean varios productos estándar.
        Producto prod1 = new Producto("Producto A", 2.0, 10.0);
        Producto prod2 = new Producto("Producto B", 3.0, 15.0);
        // Producto con volumen elevado para forzar error por falta de capacidad.
        Producto prod3 = new Producto("Producto C", 6.0, 5.0); // 2.0 + 3.0 + 6.0 = 11 > capacidad

        // Se crea un contenedor estándar (para productos) con capacidad de 10 m3.
        Contenedor<Producto> contenedorEstandar = new Contenedor<>(10.0);
        System.out.println("=== Contenedor Estándar ===");
        if (contenedorEstandar.guardar(prod1))
            System.out.println("Guardado: " + prod1.nombre());
        if (contenedorEstandar.guardar(prod2))
            System.out.println("Guardado: " + prod2.nombre());
        // Este producto no debería caber, ya que excede la capacidad restante.
        if (!contenedorEstandar.guardar(prod3))
            System.out.println("No se pudo guardar: " + prod3.nombre() + " (capacidad insuficiente)");
        System.out.println(contenedorEstandar);

        // ====================================================
        // 2. Prueba de cargas especiales y contenedores especializados
        // ====================================================
        // Se crean cargas especiales: seres vivos y productos tóxicos.
        SerVivo ser1 = new SerVivo("SerVivo A", 1.0, 20.0);
        SerVivo ser2 = new SerVivo("SerVivo B", 1.5, 25.0);
        Toxico tox1 = new Toxico("Tóxico A", 0.8, 30.0);

        // Se crean contenedores especializados para seres vivos y tóxicos.
        Contenedor<SerVivo> contenedorSeresVivos = new Contenedor<>(5.0);
        Contenedor<Toxico> contenedorToxicos = new Contenedor<>(5.0);

        System.out.println("=== Contenedor de Seres Vivos ===");
        if (contenedorSeresVivos.guardar(ser1))
            System.out.println("Guardado: " + ser1.nombre());
        if (contenedorSeresVivos.guardar(ser2))
            System.out.println("Guardado: " + ser2.nombre());
        System.out.println(contenedorSeresVivos);

        System.out.println("=== Contenedor de Productos Tóxicos ===");
        if (contenedorToxicos.guardar(tox1))
            System.out.println("Guardado: " + tox1.nombre());
        System.out.println(contenedorToxicos);

        // ====================================================
        // 3. Prueba del camión, que acepta cualquier carga (productos o contenedores)
        // ====================================================
        // Se crea un camión con gran capacidad (50 m3).
        Camion camion = new Camion(50.0);
        System.out.println("=== Camión ===");
        if (camion.guardar(prod1))
            System.out.println("Guardado en camión: " + prod1.nombre());
        // Se añade el contenedor estándar (que ya contiene productos).
        if (camion.guardar(contenedorEstandar))
            System.out.println("Guardado en camión: contenedor estándar");
        // Se añade el contenedor de seres vivos.
        if (camion.guardar(contenedorSeresVivos))
            System.out.println("Guardado en camión: contenedor de seres vivos");
        // Se añade el contenedor de tóxicos.
        if (camion.guardar(contenedorToxicos))
            System.out.println("Guardado en camión: contenedor de productos tóxicos");
        System.out.println(camion);

        // ====================================================
        // 4. Líneas comentadas para comprobar restricciones de compilación
        // Estas líneas, de ser descomentadas, deben generar errores de compilación
        // debido a que intentan guardar tipos incompatibles.
        // ====================================================

        // a) Intentar guardar un SerVivo en un contenedor de Producto (contenedor estándar):
        //Contenedor<Producto> errorContenedor = new Contenedor<>(10.0);
        //errorContenedor.guardar(ser1); 
        // Error: No se puede guardar una carga de tipo SerVivo en un contenedor que espera Producto.

        // b) Intentar guardar un Camion en un contenedor de Producto:
        //errorContenedor.guardar(camion);
        // Error: Camion no es un Producto, aunque Camion pueda ser almacenado en un camión.

        // c) Intentar guardar un Producto en un contenedor especializado para SerVivo:
        // Contenedor<SerVivo> errorContenedorSeres = new Contenedor<>(5.0);
        // errorContenedorSeres.guardar(prod2);
        // Error: Producto no es un SerVivo.
    }
}
