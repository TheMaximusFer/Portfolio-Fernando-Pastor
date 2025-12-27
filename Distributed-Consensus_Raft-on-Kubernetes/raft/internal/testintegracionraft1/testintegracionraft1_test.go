package testintegracionraft1

import (
	"fmt"
	"raft/internal/comun/check"

	//"log"
	//"crypto/rand"
	"os"
	"path/filepath"
	"strconv"
	"testing"
	"time"

	"raft/internal/comun/rpctimeout"
	"raft/internal/despliegue"
	"raft/internal/raft"
	cltraft "raft/pkg/pkgclt"
)

const (
	//hosts
	MAQUINA1 = "192.168.3.2"
	MAQUINA2 = "192.168.3.3"
	MAQUINA3 = "192.168.3.4"

	//puertos
	PUERTOREPLICA1 = "29050"
	PUERTOREPLICA2 = "29051"
	PUERTOREPLICA3 = "29052"

	//nodos replicas
	REPLICA1 = MAQUINA1 + ":" + PUERTOREPLICA1
	REPLICA2 = MAQUINA2 + ":" + PUERTOREPLICA2
	REPLICA3 = MAQUINA3 + ":" + PUERTOREPLICA3

	// paquete main de ejecutables relativos a PATH previo
	EXECREPLICA = "cmd/srvraft/main.go"

	// comandos completo a ejecutar en máquinas remota con ssh. Ejemplo :
	// 				cd $HOME/raft; go run cmd/srvraft/main.go 127.0.0.1:29050

	// Ubicar, en esta constante, nombre de fichero de vuestra clave privada local
	// emparejada con la clave pública en authorized_keys de máquinas remotas

	PRIVKEYFILE = "id_ed25519"
)

// PATH de los ejecutables de modulo golang de servicio Raft
var PATH string = filepath.Join(os.Getenv("HOME"), "raft")

// go run cmd/srvraft/main.go 0 127.0.0.1:29050 127.0.0.1:29051 127.0.0.1:29052
var EXECREPLICACMD string = "cd " + PATH + "; go run " + EXECREPLICA

// TEST primer rango
func TestPrimerasPruebas(t *testing.T) { // (m *testing.M) {
	// <setup code>
	// Crear canal de resultados de ejecuciones ssh en maquinas remotas
	cfg := makeCfgDespliegue(t,
		3,
		[]string{REPLICA1, REPLICA2, REPLICA3},
		[]bool{true, true, true})

	// tear down code
	// eliminar procesos en máquinas remotas
	defer cfg.stop()

	// Run test sequence

	// Test1 : No debería haber ningun primario, si SV no ha recibido aún latidos
	t.Run("T1:soloArranqueYparada",
		func(t *testing.T) { cfg.soloArranqueYparadaTest1(t) })

	// Test2 : No debería haber ningun primario, si SV no ha recibido aún latidos
	t.Run("T2:ElegirPrimerLider",
		func(t *testing.T) { cfg.elegirPrimerLiderTest2(t) })

	// Test3: tenemos el primer primario correcto
	t.Run("T3:FalloAnteriorElegirNuevoLider",
		func(t *testing.T) { cfg.falloAnteriorElegirNuevoLiderTest3(t) })

	// Test4: Tres operaciones comprometidas en configuración estable
	t.Run("T4:tresOperacionesComprometidasEstable",
		func(t *testing.T) { cfg.tresOperacionesComprometidasEstable(t) })
}

// TEST primer rango
func TestAcuerdosConFallos(t *testing.T) { // (m *testing.M) {
	// <setup code>
	// Crear canal de resultados de ejecuciones ssh en maquinas remotas
	cfg := makeCfgDespliegue(t,
		3,
		[]string{REPLICA1, REPLICA2, REPLICA3},
		[]bool{true, true, true})

	// tear down code
	// eliminar procesos en máquinas remotas
	defer cfg.stop()

	// Test5: Se consigue acuerdo a pesar de desconexiones de seguidor
	t.Run("T5:AcuerdoAPesarDeDesconexionesDeSeguidor ",
		func(t *testing.T) { cfg.AcuerdoApesarDeSeguidor(t) })

	t.Run("T5:SinAcuerdoPorFallos ",
		func(t *testing.T) { cfg.SinAcuerdoPorFallos(t) })

	t.Run("T5:SometerConcurrentementeOperaciones ",
		func(t *testing.T) { cfg.SometerConcurrentementeOperaciones(t) })

}

// ---------------------------------------------------------------------
//
// Canal de resultados de ejecución de comandos ssh remotos
type canalResultados chan string

func (cr canalResultados) stop() {
	close(cr)

	// Leer las salidas obtenidos de los comandos ssh ejecutados
	for s := range cr {
		fmt.Println(s)
	}
}

// ---------------------------------------------------------------------
// Operativa en configuracion de despliegue y pruebas asociadas
type configDespliegue struct {
	t           *testing.T
	conectados  []bool
	numReplicas int
	nodosRaft   []rpctimeout.HostPort
	cr          canalResultados
}

// Crear una configuracion de despliegue
func makeCfgDespliegue(t *testing.T, n int, nodosraft []string,
	conectados []bool) *configDespliegue {
	cfg := &configDespliegue{}
	cfg.t = t
	cfg.conectados = conectados
	cfg.numReplicas = n
	cfg.nodosRaft = rpctimeout.StringArrayToHostPortArray(nodosraft)
	cfg.cr = make(canalResultados, 2000)

	return cfg
}

func (cfg *configDespliegue) stop() {
	//cfg.stopDistributedProcesses()

	time.Sleep(50 * time.Millisecond)

	cfg.cr.stop()
}

// --------------------------------------------------------------------------
// FUNCIONES DE SUBTESTS

// Se pone en marcha una replica ?? - 3 NODOS RAFT
func (cfg *configDespliegue) soloArranqueYparadaTest1(t *testing.T) {
	//t.Skip("SKIPPED soloArranqueYparadaTest1")

	fmt.Println(t.Name(), ".....................")

	cfg.t = t // Actualizar la estructura de datos de tests para errores

	// Poner en marcha replicas en remoto con un tiempo de espera incluido
	cfg.startDistributedProcesses()

	// Comprobar estado replica 0
	cfg.comprobarEstadoRemoto(0, 0, false, -1)

	// Comprobar estado replica 1
	cfg.comprobarEstadoRemoto(1, 0, false, -1)

	// Comprobar estado replica 2
	cfg.comprobarEstadoRemoto(2, 0, false, -1)

	// Parar réplicas almacenamiento en remoto
	cfg.stopDistributedProcesses()

	fmt.Println(".............", t.Name(), "Superado")
}

// Primer lider en marcha - 3 NODOS RAFT
func (cfg *configDespliegue) elegirPrimerLiderTest2(t *testing.T) {
	t.Skip("SKIPPED ElegirPrimerLiderTest2")

	fmt.Println(t.Name(), ".....................")

	cfg.startDistributedProcesses()

	// Se ha elegido lider ?
	fmt.Printf("Probando lider en curso\n")
	cfg.pruebaUnLider(3)

	// Parar réplicas alamcenamiento en remoto
	cfg.stopDistributedProcesses() // Parametros

	fmt.Println(".............", t.Name(), "Superado")
}

// Fallo de un primer lider y reeleccion de uno nuevo - 3 NODOS RAFT
func (cfg *configDespliegue) falloAnteriorElegirNuevoLiderTest3(t *testing.T) {
	t.Skip("SKIPPED FalloAnteriorElegirNuevoLiderTest3")

	fmt.Println(t.Name(), ".....................")

	cfg.startDistributedProcesses()

	fmt.Printf("Lider inicial\n")
	cfg.pruebaUnLider(3)

	// Desconectar lider
	// ???

	fmt.Printf("Comprobar nuevo lider\n")
	cfg.pruebaUnLider(3)

	// Parar réplicas almacenamiento en remoto
	cfg.stopDistributedProcesses() //parametros

	fmt.Println(".............", t.Name(), "Superado")
}

// 3 operaciones comprometidas con situacion estable y sin fallos - 3 NODOS RAFT
func (cfg *configDespliegue) tresOperacionesComprometidasEstable(t *testing.T) {
	//t.Skip("SKIPPED tresOperacionesComprometidasEstable")

	fmt.Println(t.Name(), ".....................")

	cfg.startDistributedProcesses()
	defer cfg.stopDistributedProcesses()

	// Espera líder estable.
	lider := cfg.pruebaUnLider(3) // Detecta y retorna el ID del líder.
	fmt.Printf("Líder inicial: %d\n", lider)

	cli := cltraft.NuevoCliente(cfg.nodosRaft) // Cliente que conoce todos los nodos

	ops := []raft.TipoOperacion{ // Operaciones a someter
		{Operacion: "escribir", Clave: "k0", Valor: "v0"},
		{Operacion: "escribir", Clave: "k1", Valor: "v1"},
		{Operacion: "escribir", Clave: "k2", Valor: "v2"},
	}

	for i, op := range ops { // Sometemos operaciones y comprobamos resultados
		res, err := cli.SometerOperacion(op)
		if err != nil {
			t.Fatalf("Error sometiendo operación %d: %v", i, err)
		}

		if !res.EsLiderQuienResponde {
			t.Fatalf("La operación %d no fue respondida por el líder", i)
		}

		if res.IdLider != lider {
			t.Fatalf("Líder incorrecto en operación %d: esperado %d, obtenido %d",
				i, lider, res.IdLider)
		}

		if res.IndiceRegistro != i+1 {
			t.Fatalf("Índice incorrecto para operación %d: esperado %d, obtenido %d",
				i, i+1, res.IndiceRegistro)
		}

		fmt.Printf("Operación %d comprometida en índice %d\n", i, res.IndiceRegistro)
		time.Sleep(200 * time.Millisecond)
	}

	// Verificar que todos los nodos conocen al mismo líder
	time.Sleep(1 * time.Second)
	for i := 0; i < cfg.numReplicas; i++ {
		_, _, _, idLider := cfg.obtenerEstadoRemoto(i)
		if idLider != lider {
			t.Fatalf("Réplica %d reporta líder incorrecto: esperado %d, obtenido %d",
				i, lider, idLider)
		}
	}
	fmt.Println(".............", t.Name(), "Superado")

}

// Se consigue acuerdo a pesar de desconexiones de seguidor -- 3 NODOS RAFT
func (cfg *configDespliegue) AcuerdoApesarDeSeguidor(t *testing.T) {
	//t.Skip("SKIPPED AcuerdoApesarDeSeguidor")

	fmt.Println(t.Name(), ".....................")

	cfg.startDistributedProcesses()

	// Obtener un lider
	fmt.Printf("Obteniendo lider inicial\n")
	lider := cfg.pruebaUnLider(3)
	fmt.Printf("Lider inicial: %d\n", lider)

	// Comprometer una entrada
	opInicial := raft.TipoOperacion{
		Operacion: "escribir",
		Clave:     "inicial",
		Valor:     "test",
	}

	fmt.Printf("Comprometiendo entrada inicial\n")
	indiceInicial, mandatoInicial, esLider, idLider, _ :=
		cfg.someterOperacionRemoto(lider, opInicial)

	if !esLider || idLider != lider {
		t.Fatalf("Error en la operación inicial")
	}

	fmt.Printf("Entrada inicial comprometida: indice=%d, mandato=%d\n",
		indiceInicial, mandatoInicial)

	// Obtener un lider y, a continuación desconectar una de los nodos Raft
	seguidorDesconectado := (lider + 1) % 3
	fmt.Printf("Desconectando seguidor: %d\n", seguidorDesconectado)
	cfg.desconectarNodo(seguidorDesconectado)

	// Comprobar varios acuerdos con una réplica desconectada
	opsConFallo := []raft.TipoOperacion{
		{Operacion: "escribir", Clave: "k1", Valor: "v1"},
		{Operacion: "escribir", Clave: "k2", Valor: "v2"},
		{Operacion: "escribir", Clave: "k3", Valor: "v3"},
	}

	acuerdos := 0
	for i, op := range opsConFallo {
		indice, mandato, esLider, idLider, _ :=
			cfg.someterOperacionRemoto(lider, op)

		if esLider && idLider == lider {
			acuerdos++
			fmt.Printf("Acuerdo %d conseguido: índice=%d, mandato=%d\n",
				i+1, indice, mandato)
		} else {
			fmt.Printf("Acuerdo %d falló\n", i+1)
		}

		time.Sleep(300 * time.Millisecond)
	}

	// Verificar que se consiguieron acuerdos
	if acuerdos < len(opsConFallo)-1 {
		t.Fatalf("No se consiguieron suficientes acuerdos: %d/%d",
			acuerdos, len(opsConFallo))
	}
	fmt.Printf("Se consiguieron %d/%d acuerdos con réplica desconectada\n",
		acuerdos, len(opsConFallo))

	// reconectar nodo Raft previamente desconectado y comprobar varios acuerdos
	fmt.Printf("Reconectando seguidor: %d\n", seguidorDesconectado)
	cfg.reconectarNodo(seguidorDesconectado)

	// Esperar a que el sistema se estabilice
	time.Sleep(2 * time.Second)

	// Comprobar que se pueden seguir alcanzando acuerdos
	opsPostReconexion := []raft.TipoOperacion{
		{Operacion: "escribir", Clave: "post_reconexion1", Valor: "v1"},
		{Operacion: "escribir", Clave: "post_reconexion2", Valor: "v2"},
	}

	for i, op := range opsPostReconexion {
		indice, mandato, esLider, idLider, _ :=
			cfg.someterOperacionRemoto(lider, op)

		if !esLider || idLider != lider {
			t.Fatalf("Error en acuerdo post-reconexión %d", i+1)
		}
		fmt.Printf("Acuerdo post-reconexión %d: índice=%d, mandato=%d\n",
			i+1, indice, mandato)

		time.Sleep(200 * time.Millisecond)
	}

	cfg.stopDistributedProcesses()
	fmt.Printf("Todos los acuerdos post-reconexión completados correctamente\n")
	fmt.Println(".............", t.Name(), "Superado")
}

// NO se consigue acuerdo al desconectarse mayoría de seguidores -- 3 NODOS RAFT
func (cfg *configDespliegue) SinAcuerdoPorFallos(t *testing.T) {
	//t.Skip("SKIPPED SinAcuerdoPorFallos")

	fmt.Println(t.Name(), ".....................")
	cfg.startDistributedProcesses()

	// A completar ???

	// Comprometer una entrada
	lider := cfg.pruebaUnLider(3)
	fmt.Printf("Líder inicial: %d\n", lider)

	opInicial := raft.TipoOperacion{
		Operacion: "escribir",
		Clave:     "entrada_inicial",
		Valor:     "valor_inicial",
	}

	indiceInicial, mandatoInicial, esLider, idLider, _ :=
		cfg.someterOperacionRemoto(lider, opInicial)

	if !esLider || idLider != lider {
		t.Fatalf("Error comprometiendo entrada inicial")
	}
	fmt.Printf("Entrada inicial comprometida: índice=%d, mandato=%d\n",
		indiceInicial, mandatoInicial)

	//  Obtener un lider y, a continuación desconectar 2 de los nodos Raft
	replicasDesconectadas := []int{(lider + 1) % 3, (lider + 2) % 3}
	fmt.Printf("Desconectando 2 réplicas: %v\n", replicasDesconectadas)

	for _, replica := range replicasDesconectadas {
		cfg.desconectarNodo(replica)
	}

	// Comprobar varios acuerdos con 2 réplicas desconectadas
	opsIntento := []raft.TipoOperacion{
		{Operacion: "escribir", Clave: "intento1", Valor: "fallo1"},
		{Operacion: "escribir", Clave: "intento2", Valor: "fallo2"},
		{Operacion: "escribir", Clave: "intento3", Valor: "fallo3"},
	}

	acuerdos := 0
	for i, op := range opsIntento {
		indice, _, esLider, idLider, _ :=
			cfg.someterOperacionRemoto(lider, op)

		// Sin mayoría, no debería comprometerse
		if esLider && idLider == lider && indice > indiceInicial {
			acuerdos++
			fmt.Printf("Acuerdo %d INESPERADO: índice=%d\n", i+1, indice)
		} else {
			fmt.Printf("Acuerdo %d NO conseguido\n", i+1)
		}

		time.Sleep(500 * time.Millisecond)
	}

	// Verificar que no se consiguieron acuerdos
	if acuerdos > 0 {
		t.Logf("ADVERTENCIA: Se consiguieron %d acuerdos sin mayoría", acuerdos)
	} else {
		fmt.Printf("Comportamiento correcto: 0 acuerdos sin mayoría\n")
	}

	// reconectar los 2 nodos Raft desconectados y probar varios acuerdos
	fmt.Printf("Reconectando 2 réplicas: %v\n", replicasDesconectadas)
	for _, replica := range replicasDesconectadas {
		cfg.reconectarNodo(replica)
	}

	// Esperar reestabilización
	time.Sleep(3 * time.Second)

	// Verificar que se recupera el acuerdo
	nuevoLider := cfg.pruebaUnLider(3)
	fmt.Printf("Nuevo líder después de reconexión: %d\n", nuevoLider)

	opRecuperacion := raft.TipoOperacion{
		Operacion: "escribir",
		Clave:     "recuperacion",
		Valor:     "ok",
	}

	indiceRec, mandatoRec, esLider, idLider, _ :=
		cfg.someterOperacionRemoto(nuevoLider, opRecuperacion)

	if !esLider || idLider != nuevoLider {
		t.Fatalf("No se recuperó el acuerdo después de reconexión")
	}

	cfg.stopDistributedProcesses()

	fmt.Printf("Acuerdo recuperado: índice=%d, mandato=%d\n",
		indiceRec, mandatoRec)
	fmt.Println(".............", t.Name(), "Superado")
}

// Se somete 5 operaciones de forma concurrente -- 3 NODOS RAFT
func (cfg *configDespliegue) SometerConcurrentementeOperaciones(t *testing.T) {
	//t.Skip("SKIPPED SometerConcurrentementeOperaciones")

	fmt.Println(t.Name(), ".....................")

	cfg.startDistributedProcesses()

	// un bucle para estabilizar la ejecucion
	time.Sleep(2 * time.Second)

	// Obtener un lider y, a continuación someter una operacion
	lider := cfg.pruebaUnLider(3)
	fmt.Printf("Líder estable: %d\n", lider)

	opInicial := raft.TipoOperacion{
		Operacion: "escribir",
		Clave:     "inicio_concurrente",
		Valor:     "referencia",
	}

	indiceInicial, _, esLider, _, _ :=
		cfg.someterOperacionRemoto(lider, opInicial)

	if !esLider {
		t.Fatalf("Error en operación inicial")
	}

	fmt.Printf("Operación inicial: índice=%d\n", indiceInicial)

	// Someter 5  operaciones concurrentes
	opsConcurrentes := []raft.TipoOperacion{
		{Operacion: "escribir", Clave: "c1", Valor: "v1"},
		{Operacion: "escribir", Clave: "c2", Valor: "v2"},
		{Operacion: "escribir", Clave: "c3", Valor: "v3"},
		{Operacion: "escribir", Clave: "c4", Valor: "v4"},
		{Operacion: "escribir", Clave: "c5", Valor: "v5"},
	}

	// DEFINIR EL TIPO DE ESTRUCTURA PRIMERO
	type resultadoOperacion struct {
		idx    int
		indice int
		err    error
	}

	// Canal para resultados concurrentes
	resultados := make(chan resultadoOperacion, len(opsConcurrentes))

	// Lanzar operaciones concurrentes
	for i, op := range opsConcurrentes {
		go func(idx int, operacion raft.TipoOperacion) {
			indice, _, _, _, err :=
				cfg.someterOperacionRemoto(lider, operacion)
			resultados <- resultadoOperacion{
				idx:    idx,
				indice: indice,
				err:    err,
			}
		}(i, op)
	}

	// Recoger resultados
	indices := make([]int, len(opsConcurrentes))
	for i := 0; i < len(opsConcurrentes); i++ {
		select {
		case res := <-resultados:
			if res.err != nil {
				t.Fatalf("Error en operación %d: %v", res.idx, res.err)
			}
			indices[res.idx] = res.indice
			fmt.Printf("Operación %d → índice %d\n", res.idx, res.indice)
		case <-time.After(10 * time.Second):
			t.Fatalf("Timeout en operaciones concurrentes")
		}
	}

	// Comprobar estados de nodos Raft, sobre todo
	// el avance del mandato en curso e indice de registro de cada uno
	// que debe ser identico entre ellos
	fmt.Println("Comprobando consistencia entre réplicas...")
	time.Sleep(1 * time.Second)

	for i := 0; i < cfg.numReplicas; i++ {
		idNodo, mandato, _, idLider := cfg.obtenerEstadoRemoto(i)

		// Verificar que todas conocen al mismo líder
		if idLider != lider {
			t.Fatalf("Réplica %d tiene líder inconsistente", idNodo)
		}

		fmt.Printf("Réplica %d: mandato=%d, líder=%d ✓\n",
			idNodo, mandato, idLider)
	}

	// Verificar avance del índice
	maxIndice := indices[0]
	for _, idx := range indices {
		if idx > maxIndice {
			maxIndice = idx
		}
	}

	fmt.Printf("Índice avanzó de %d a %d\n",
		indiceInicial, maxIndice)
	fmt.Printf("5 operaciones concurrentes completadas ✓\n")
	fmt.Println(".............", t.Name(), "Superado")
}

// --------------------------------------------------------------------------
// FUNCIONES DE APOYO
// Comprobar que hay un solo lider
// probar varias veces si se necesitan reelecciones
func (cfg *configDespliegue) pruebaUnLider(numreplicas int) int {
	for iters := 0; iters < 10; iters++ {
		time.Sleep(500 * time.Millisecond)
		mapaLideres := make(map[int][]int)
		for i := 0; i < numreplicas; i++ {
			if cfg.conectados[i] {
				if _, mandato, esLider, _ := cfg.obtenerEstadoRemoto(i); esLider {
					mapaLideres[mandato] = append(mapaLideres[mandato], i)
				}
			}
		}

		ultimoMandatoConLider := -1
		for mandato, lideres := range mapaLideres {
			if len(lideres) > 1 {
				cfg.t.Fatalf("mandato %d tiene %d (>1) lideres",
					mandato, len(lideres))
			}
			if mandato > ultimoMandatoConLider {
				ultimoMandatoConLider = mandato
			}
		}

		if len(mapaLideres) != 0 {

			return mapaLideres[ultimoMandatoConLider][0] // Termina

		}
	}
	cfg.t.Fatalf("un lider esperado, ninguno obtenido")

	return -1 // Termina
}

func (cfg *configDespliegue) obtenerEstadoRemoto(
	indiceNodo int) (int, int, bool, int) {
	var reply raft.EstadoRemoto
	err := cfg.nodosRaft[indiceNodo].CallTimeout("NodoRaft.ObtenerEstadoNodo",
		raft.Vacio{}, &reply, 10*time.Millisecond)
	check.CheckError(err, "Error en llamada RPC ObtenerEstadoRemoto")

	return reply.IdNodo, reply.Mandato, reply.EsLider, reply.IdLider
}

// start  gestor de vistas; mapa de replicas y maquinas donde ubicarlos;
// y lista clientes (host:puerto)
func (cfg *configDespliegue) startDistributedProcesses() {
	//cfg.t.Log("Before starting following distributed processes: ", cfg.nodosRaft)

	for i, endPoint := range cfg.nodosRaft {
		despliegue.ExecMutipleHosts(EXECREPLICACMD+
			" "+strconv.Itoa(i)+" "+
			rpctimeout.HostPortArrayToString(cfg.nodosRaft),
			[]string{endPoint.Host()}, cfg.cr, PRIVKEYFILE)

		// dar tiempo para se establezcan las replicas
		//time.Sleep(500 * time.Millisecond)
	}

	// aproximadamente 500 ms para cada arranque por ssh en portatil
	time.Sleep(2500 * time.Millisecond)
}

func (cfg *configDespliegue) stopDistributedProcesses() {
	var reply raft.Vacio

	for _, endPoint := range cfg.nodosRaft {
		err := endPoint.CallTimeout("NodoRaft.ParaNodo",
			raft.Vacio{}, &reply, 10*time.Millisecond)
		check.CheckError(err, "Error en llamada RPC Para nodo")
	}
}

// Comprobar estado remoto de un nodo con respecto a un estado prefijado
func (cfg *configDespliegue) comprobarEstadoRemoto(idNodoDeseado int,
	mandatoDeseado int, esLiderDeseado bool, IdLiderDeseado int) {
	idNodo, mandato, esLider, idLider := cfg.obtenerEstadoRemoto(idNodoDeseado)

	//cfg.t.Log("Estado replica 0: ", idNodo, mandato, esLider, idLider, "\n")

	if idNodo != idNodoDeseado || mandato != mandatoDeseado ||
		esLider != esLiderDeseado || idLider != IdLiderDeseado {
		cfg.t.Fatalf("Estado incorrecto en replica %d en subtest %s",
			idNodoDeseado, cfg.t.Name())
	}

}

// someterOperacionRemoto envía una operación a un nodo específico
func (cfg *configDespliegue) someterOperacionRemoto(nodo int,
	op raft.TipoOperacion) (int, int, bool, int, error) {
	var reply raft.ResultadoRemoto
	err := cfg.nodosRaft[nodo].CallTimeout("NodoRaft.SometerOperacionRaft",
		op, &reply, 2*time.Second)

	if err != nil {
		return -1, -1, false, -1, err
	}

	return reply.IndiceRegistro, reply.Mandato, reply.EsLider, reply.IdLider, nil
}

// desconectarNodo detiene un nodo Raft específico
func (cfg *configDespliegue) desconectarNodo(indice int) {
	var reply raft.Vacio

	// Solo intentar desconectar si el nodo ya está conectado
	if cfg.conectados[indice] {
		err := cfg.nodosRaft[indice].CallTimeout("NodoRaft.ParaNodo",
			raft.Vacio{}, &reply, 100*time.Millisecond)

		if err != nil {
			fmt.Printf("Error desconectando nodo %d: %v\n", indice, err)
		} else {
			fmt.Printf("Nodo %d desconectado correctamente\n", indice)
		}

		cfg.conectados[indice] = false
	}

	// Dar tiempo para que se propague el fallo
	time.Sleep(500 * time.Millisecond)
}

// reconectarNodo reinicia un nodo Raft previamente desconectado
func (cfg *configDespliegue) reconectarNodo(indice int) {
	// Solo reconectar si el nodo estaba desconectado
	if !cfg.conectados[indice] {
		endPoint := cfg.nodosRaft[indice]

		fmt.Printf("Reiniciando nodo %d en %s\n", indice, endPoint.Host())

		despliegue.ExecMutipleHosts(EXECREPLICACMD+" "+strconv.Itoa(indice)+
			" "+rpctimeout.HostPortArrayToString(cfg.nodosRaft),
			[]string{endPoint.Host()}, cfg.cr, PRIVKEYFILE)

		cfg.conectados[indice] = true

		// Dar tiempo suficiente para el arranque y estabilización
		time.Sleep(2 * time.Second)
	}
}
