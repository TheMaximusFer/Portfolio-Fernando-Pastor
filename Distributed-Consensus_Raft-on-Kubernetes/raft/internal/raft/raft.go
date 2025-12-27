package raft

//
// API
// ===
// Este es el API que vuestra implementación debe exportar
//
// nodoRaft = NuevoNodo(...)
//   Crear un nuevo servidor del grupo de elección.
//
// nodoRaft.Para()
//   Solicitar la parado de un servidor
//
// nodo.ObtenerEstado() (yo, mandato, esLider)
//   Solicitar a un nodo de elección por "yo", su mandato en curso,
//   y si piensa que es el msmo el lider
//
// nodoRaft.SometerOperacion(operacion interface()) (indice, mandato, esLider)

// type AplicaOperacion

import (
	"fmt"
	"io"
	"log"
	"os"

	//"crypto/rand"
	"math/rand"
	"sync"
	"time"

	//"net/rpc"

	"raft/internal/comun/rpctimeout"
)

type Estado int

const (
	// Constante para fijar valor entero no inicializado
	IntNOINICIALIZADO = -1

	//  false deshabilita por completo los logs de depuracion
	// Aseguraros de poner kEnableDebugLogs a false antes de la entrega
	kEnableDebugLogs = true

	// Poner a true para logear a stdout en lugar de a fichero
	kLogToStdout = true

	// Cambiar esto para salida de logs en un directorio diferente
	kLogOutputDir = "./logs_raft/"

	// Estados posibles de un nodo Raft.
	Follower  Estado = 1
	Candidate Estado = 2
	Leader    Estado = 3

	AutoEleccionesAlArrancar = true // SOLO PONER A "false" PARA TEST-1
)

type TipoOperacion struct {
	Operacion string // La operaciones posibles son "leer" y "escribir"
	Clave     string
	Valor     string // en el caso de la lectura Valor = ""
}

type entrada struct {
	Term      int
	Indice    int
	Operacion TipoOperacion
}

// A medida que el nodo Raft conoce las operaciones de las  entradas de registro
// comprometidas, envía un AplicaOperacion, con cada una de ellas, al canal
// "canalAplicar" (funcion NuevoNodo) de la maquina de estados
type AplicaOperacion struct {
	Indice    int // en la entrada de registro
	Operacion TipoOperacion
	Respuesta chan string
}

// Tipo de dato Go que representa un solo nodo (réplica) de raft
type NodoRaft struct {
	Mux     sync.Mutex            // Mutex para proteger acceso a estado compartido.
	Nodos   []rpctimeout.HostPort // Host:Port de todos los nodos (réplicas) Raft, en mismo orden.
	Yo      int                   // Índice de este nodos en campo array "nodos".
	IdLider int                   // Índice del lider.
	Logger  *log.Logger           // Utilización opcional de este logger para depuración, cada nodo Raft tiene su propio registro de trazas (logs).

	// ----  Persistent state on all servers: ---
	currentTerm int       	// Mandato actual.
	votedFor    int       	// A quién se votó en currentTerm (-1 si nadie).
	log         []entrada 	// Registro de entradas, cada entrada contiene el mandato y la operación.
	// ----- Volatile state on all servers: -----
	commitIndex int 	   	// Mayor índice comprometido
	lastApplied int 	   	// Mayor índice aplicado a la máquina de estados
	// ------- Volatile state on leaders: -------
	nextIndex  []int 		// Para cada servidor, índice de la siguiente entrada a enviar.
	matchIndex []int 		// Para cada servidor, índice del mayor registro conocido como replicado.
	// ------------------------------------------
	estado                Estado               // Indica si soy seguidor, candidato o lider.
	latidos               chan bool            // Temporizador para latidos del lider.
	votosRecibidos        int                  // Contador de votos en elección.
	finEleccion           chan bool            // Para señalar fin de elección.
	quit                  chan bool            // Para parar gorutinas.
	inicioEleccion        time.Time            // Marca de tiempo del inicio de la elección
	enEleccion            bool                 // Indica si el nodo está en proceso de elección
	canalAplicarOperacion chan AplicaOperacion // siguiente práctica
	canalesRespuesta 	  []chan string  	   // La posición i del array corresponderá al log con índice i (p5).
}

// Creacion de un nuevo nodo de eleccion
//
// Tabla de <Direccion IP:puerto> de cada nodo incluido a si mismo.
//
// <Direccion IP:puerto> de este nodo esta en nodos[yo]
//
// Todos los arrays nodos[] de los nodos tienen el mismo orden

// canalAplicar es un canal donde, en la practica 5, se recogerán las
// operaciones a aplicar a la máquina de estados. Se puede asumir que
// este canal se consumira de forma continúa.
//
// NuevoNodo() debe devolver resultado rápido, por lo que se deberían
// poner en marcha Gorutinas para trabajos de larga duracion

// Devuelve el nodo Raft incializado y lanza la gouroutina de gestión de estados.
func NuevoNodo(nodos []rpctimeout.HostPort, yo int,
	canalAplicarOperacion chan AplicaOperacion) *NodoRaft {
	nr := &NodoRaft{}
	nr.Nodos = nodos
	nr.Yo = yo
	nr.IdLider = -1

	//Semilla aleatoria gracias al número de nanosegundos transcurridos desde
	// la “época Unix” (1 de enero de 1970 00:00:00 UTC) hasta el instante actual.
	// (En nanosegundos para evitar al máximo que dos nodos tengan misma semilla).
	//rand.Seed(time.Now().UnixNano())

	nr.inicializarLogger(nodos, yo)
	nr.inicializarEstado(nodos)
	nr.inicializarCanales(canalAplicarOperacion)

	if AutoEleccionesAlArrancar {
		go func() {
			// margen para que main registre RPC, haga Listen, etc.
			time.Sleep(800 * time.Millisecond)
			nr.actualizarEstados()
		}()
	}

	return nr
}

func (nr *NodoRaft) inicializarLogger(nodos []rpctimeout.HostPort, yo int) {
	if kEnableDebugLogs {
		nombreNodo := nodos[yo].Host() + "_" + nodos[yo].Port()
		logPrefix := nombreNodo

		fmt.Println("LogPrefix: ", logPrefix)

		if kLogToStdout {
			nr.Logger = log.New(os.Stdout, nombreNodo+" -->> ",
				log.Lmicroseconds|log.Lshortfile)
		} else {
			err := os.MkdirAll(kLogOutputDir, os.ModePerm)
			if err != nil {
				panic(err.Error())
			}
			logOutputFile, err := os.OpenFile(fmt.Sprintf("%s/%s.txt",
				kLogOutputDir, logPrefix), os.O_RDWR|os.O_CREATE|os.O_TRUNC, 0755)
			if err != nil {
				panic(err.Error())
			}
			nr.Logger = log.New(logOutputFile,
				logPrefix+" -> ", log.Lmicroseconds|log.Lshortfile)
		}
		nr.Logger.Println("logger initialized")
	} else {
		nr.Logger = log.New(io.Discard, "", 0)
	}
}

func (nr *NodoRaft) inicializarEstado(nodos []rpctimeout.HostPort) {
	nr.currentTerm = 0
	nr.votedFor = -1
	nr.log = make([]entrada, 0, 64)
	nr.commitIndex = 0
	nr.lastApplied = 0
	nr.nextIndex = make([]int, len(nodos))
	nr.matchIndex = make([]int, len(nodos))
	nr.estado = Follower

	// Inicializamos nextIndex y matchIndex para líderes.
	for i := range nr.nextIndex {
		nr.nextIndex[i] = 1
		nr.matchIndex[i] = 0
	}
}

func (nr *NodoRaft) inicializarCanales(canalAplicarOperacion chan AplicaOperacion) {
	nr.latidos = make(chan bool, 1)
	nr.votosRecibidos = 0
	nr.finEleccion = make(chan bool, 1)
	nr.quit = make(chan bool, 1)
	nr.enEleccion = false
	nr.inicioEleccion = time.Now()
	nr.canalAplicarOperacion = canalAplicarOperacion
	nr.canalesRespuesta = make([]chan string, 0, 100) //p5
}

/*
 * Bucle principal de actualización de estados del nodo Raft.
 * El bucle gestiona los estados Follower, Candidate y Leader. Y sus respectivas
 * transiciones. Además, implementa un watchdog que fuerza la salida del proceso
 * si un nodo candidato no consigue líder en 2.5 segundos.
 */
func (nr *NodoRaft) actualizarEstados() {
	for {
		select {
		case <-nr.quit:
			return
		default:
		}

		nr.Mux.Lock()
		estado := nr.estado

		if nr.verificarWatchdog() {
			nr.Mux.Unlock()
			os.Exit(1)
		}

		nr.actualizarEstadoEleccion()
		nr.Mux.Unlock()

		nr.Logger.Printf(
			"[Nodo %d] WATCHDOG suicidio: estado=%d term=%d enEleccion=%v"+
				" inicioEleccion=%v elapsed=%v votos=%d IdLider=%d\n", nr.Yo,
			nr.estado, nr.currentTerm, nr.enEleccion, nr.inicioEleccion,
			time.Since(nr.inicioEleccion), nr.votosRecibidos, nr.IdLider)

		nr.ejecutarBucleEstado(estado)
	}
}

// Watchdog: Si no somos líder y estamos en proceso de elección, miramos
// cuánto llevamos buscando líder.
// antes: no devolvía nada y hacía Unlock()+Exit
func (nr *NodoRaft) verificarWatchdog() bool {
	if nr.estado != Leader && nr.enEleccion {
		if time.Since(nr.inicioEleccion) > 2500*time.Millisecond {
			return true
		}
	}
	return false
}

func (nr *NodoRaft) actualizarEstadoEleccion() {
	// Si somos líder, podemos poner enEleccion a false.
	if nr.estado == Leader {
		nr.enEleccion = false
	}

	if nr.estado == Candidate && !nr.enEleccion {
		nr.enEleccion = true
		nr.inicioEleccion = time.Now()
	}
}

// Ejecutamos el bucle correspondiente al estado actual.
func (nr *NodoRaft) ejecutarBucleEstado(estado Estado) {
	switch estado {
	case Follower:
		nr.bucleFollower()
	case Candidate:
		nr.bucleCandidate()
	case Leader:
		nr.bucleLeader()
	}
}

/*
 * Auxiliar de actualizarEstados():
 * Bucle de estado Follower.
 * Espera latidos del líder o timeout para iniciar elección.
 */
func (nr *NodoRaft) bucleFollower() {
	nr.Logger.Printf("[Nodo %d] ENTER bucleFollower(): term=%d estado=%d"+
		" enEleccion=%v IdLider=%d\n", nr.Yo, nr.currentTerm,
		nr.estado, nr.enEleccion, nr.IdLider)

	timeout := time.Duration(150+rand.Intn(150)) * time.Millisecond
	timer := time.NewTimer(timeout)
	defer timer.Stop()

	for {
		select {
		case <-nr.quit: // Parada del nodo Raft.
			return
		case <-nr.latidos: // Latido recibido de un líder.
			if !timer.Stop() {
				<-timer.C
			}
			timer.Reset(time.Duration(150+rand.Intn(150)) * time.Millisecond)
		case <-timer.C: // Timeout sin latidos, iniciar elección.
			nr.iniciarEleccion()
			return
		}
	}
}

func (nr *NodoRaft) iniciarEleccion() {
	nr.Mux.Lock()
	nr.estado = Candidate
	nr.enEleccion = true
	nr.inicioEleccion = time.Now()
	nr.Mux.Unlock()
	nr.Logger.Printf(
		"[Nodo %d] FOLLOWER -> CANDIDATE: term=%d inicioEleccion=%v\n",
		nr.Yo, nr.currentTerm, nr.inicioEleccion)
}

/*
 * Auxiliar de actualizarEstados():
 * Bucle de estado Candidate.
 * Inicia elección, envía peticiones de voto y espera resultados.
 */
func (nr *NodoRaft) bucleCandidate() {
	nr.Mux.Lock()
	nr.currentTerm++
	nr.votedFor = nr.Yo
	nr.votosRecibidos = 1

	nr.enEleccion = true
	nr.inicioEleccion = time.Now()

	termActual := nr.currentTerm
	nr.Mux.Unlock()

	nr.Logger.Printf("[Nodo %d] ENTER bucleCandidate(): term=%d votos=%d"+
		" enEleccion=%v inicioEleccion=%v\n", nr.Yo, nr.currentTerm,
		nr.votosRecibidos, nr.enEleccion, nr.inicioEleccion)

	// Limpiar cualquier notificación vieja de mayoría
	select {
	case <-nr.finEleccion:
	default:
	}

	// Enviar peticiones de voto.
	nr.enviarPeticionesVoto(termActual)

	nr.esperarResultadoEleccion()
}

func (nr *NodoRaft) esperarResultadoEleccion() {
	timeout := time.Duration(150+rand.Intn(150)) * time.Millisecond
	timer := time.NewTimer(timeout)
	defer timer.Stop()

	for {
		select {
		case <-nr.quit:
			return
		case <-nr.latidos:
			nr.finalizarEleccion(Follower)
			return
		case <-nr.finEleccion:
			nr.procesarResultadoEleccion()
			return
		case <-timer.C:
			nr.finalizarEleccionPorTimeout()
			return
		}
	}
}

// Se acabaron las elecciones, ya hay lider.
func (nr *NodoRaft) finalizarEleccion(nuevoEstado Estado) {
	nr.Mux.Lock()
	defer nr.Mux.Unlock()
	nr.estado = nuevoEstado
	nr.enEleccion = false
}

func (nr *NodoRaft) procesarResultadoEleccion() {
	nr.Mux.Lock()

	if nr.votosRecibidos > len(nr.Nodos)/2 {
		nr.convertirseEnLider()
		nr.Mux.Unlock()
		nr.Logger.Printf(
			"[Nodo %d] CANDIDATE -> LEADER !! term=%d votos=%d\n",
			nr.Yo, nr.currentTerm, nr.votosRecibidos)
	} else {
		nr.estado = Follower
		nr.enEleccion = false
		nr.Mux.Unlock()
		nr.Logger.Printf(
			"[Nodo %d] CANDIDATE -> FOLLOWER (no mayoría) "+
				"term=%d votos=%d\n",
			nr.Yo, nr.currentTerm, nr.votosRecibidos)
	}
}

func (nr *NodoRaft) convertirseEnLider() {
	nr.estado = Leader
	nr.enEleccion = false

	ultimoIndice := len(nr.log)
	for i := range nr.nextIndex {
		nr.nextIndex[i] = ultimoIndice + 1
		nr.matchIndex[i] = 0
	}
	nr.matchIndex[nr.Yo] = ultimoIndice
	nr.nextIndex[nr.Yo] = ultimoIndice + 1
	nr.IdLider = nr.Yo
}

func (nr *NodoRaft) finalizarEleccionPorTimeout() {
	nr.Mux.Lock()
	defer nr.Mux.Unlock()
	nr.Logger.Printf(
		"[Nodo %d] CANDIDATE timeout sin mayoría. "+
			"Me rindo -> FOLLOWER. term=%d votos=%d "+
			"enEleccion=%v elapsed=%v\n",
		nr.Yo, nr.currentTerm, nr.votosRecibidos, nr.enEleccion,
		time.Since(nr.inicioEleccion))
	nr.estado = Follower
	nr.enEleccion = false
}

/*
 * Auxiliar de bucleCandidate():
 * Envía peticiones de voto a todos los nodos.
 */
func (nr *NodoRaft) enviarPeticionesVoto(Term int) {
	args := nr.prepararArgsPeticionVoto(Term)

	for i := range nr.Nodos {
		if i == nr.Yo { // Enviar peticion de voto a sí mismo no tiene sentido.
			continue
		}
		// Lanzamos una gorutina por cada petición de voto.
		// Donde se procesa la respuesta.
		go nr.enviarPeticionVotoANodo(i, args)
	}
}

func (nr *NodoRaft) prepararArgsPeticionVoto(term int) ArgsPeticionVoto {
	args := ArgsPeticionVoto{
		Term:            term,
		IdCandidato:     nr.Yo,
		UltimoLogIndice: len(nr.log),
		UltimoLogTerm:   0,
	}
	if len(nr.log) > 0 {
		args.UltimoLogTerm = nr.log[len(nr.log)-1].Term
	}
	return args
}

func (nr *NodoRaft) enviarPeticionVotoANodo(nodo int,
	args ArgsPeticionVoto) {
	reply := RespuestaPeticionVoto{}
	if nr.enviarPeticionVoto(nodo, &args, &reply) {
		nr.procesarRespuestaVoto(&reply)
	}
}

func (nr *NodoRaft) procesarRespuestaVoto(reply *RespuestaPeticionVoto) {
	nr.Mux.Lock()
	defer nr.Mux.Unlock()

	if reply.Term > nr.currentTerm {
		nr.actualizarTermino(reply.Term)
		return
	}

	if reply.ConcederVoto {
		nr.votosRecibidos++
		if nr.votosRecibidos > len(nr.Nodos)/2 {
			nr.notificarFinEleccion()
		}
	}
}

func (nr *NodoRaft) actualizarTermino(nuevoTerm int) {
	nr.currentTerm = nuevoTerm
	nr.estado = Follower
	nr.votedFor = -1
	nr.enEleccion = false
}

func (nr *NodoRaft) notificarFinEleccion() {
	select {
	case nr.finEleccion <- true:
	default:
	}
}

/*
 * Auxiliar de enviarPeticionesVoto():
 * Envía una petición de voto a un nodo específico.
 * Envía latidos cada 50 ms mientras es líder.
 */
func (nr *NodoRaft) bucleLeader() {
	timer := time.NewTimer(50 * time.Millisecond)
	defer timer.Stop()

	for {
		select {
		case <-nr.quit:
			return
		case <-timer.C:
			if nr.estado != Leader {
				return
			}
			nr.enviarLatidos()
			// Reiniciar el timer para el siguiente latido.
			timer.Reset(50 * time.Millisecond)
		}
	}
}

/*
 * Auxiliar de bucleLeader():
 * Envía latidos (AppendEntries vacíos) a todos los nodos.
 */
func (nr *NodoRaft) enviarLatidos() {

	nr.Mux.Lock()
	termLocal := nr.currentTerm
	commitLocal := nr.commitIndex
	nextIndexCopia := append([]int(nil), nr.nextIndex...)
	nr.Mux.Unlock()

	for i := range nr.Nodos {
		if i == nr.Yo { // No enviamos latido a nosotros mismos.
			continue
		}
		// Lanzamos una gorutina por cada latido. Y enviamos AppendEntries vacío, luego recibimos respuesta.
		go nr.enviarLatidoANodo(i, termLocal, commitLocal, nextIndexCopia[i])
	}
}

func (nr *NodoRaft) enviarLatidoANodo(nodo int, termLocal int, commitLocal int,
	nextIndex int) {
	args := nr.prepararArgsLatido(termLocal, commitLocal, nextIndex)
	reply := Results{}

	if nr.enviarLatido(nodo, &args, &reply) {
		nr.procesarRespuestaLatido(&reply)
	}
}

func (nr *NodoRaft) prepararArgsLatido(termLocal int, commitLocal int,
	nextIndex int) ArgAppendEntries {
	args := ArgAppendEntries{
		Term:            termLocal,
		IdLider:         nr.Yo,
		PrevioLogIndice: nextIndex - 1,
		PrevioLogTerm:   0,
		Entradas:        nil,
		LeaderCommit:    commitLocal,
	}

	if args.PrevioLogIndice > 0 && args.PrevioLogIndice <= len(nr.log) {
		args.PrevioLogTerm = nr.log[args.PrevioLogIndice-1].Term
	}

	return args
}

func (nr *NodoRaft) procesarRespuestaLatido(reply *Results) {
	nr.Mux.Lock()
	defer nr.Mux.Unlock()

	if reply.Term > nr.currentTerm {
		nr.actualizarTermino(reply.Term)
	}
}

/*
 * Auxiliar de enviarLatidos() y de replicarSeguidor():
 * Envía un AppendEntries (latido) a un nodo específico.
 * Devuelve true si la llamada RPC fue exitosa, y reply relleno.
 */
func (nr *NodoRaft) enviarLatido(nodo int, args *ArgAppendEntries,
	reply *Results) bool {
	err := nr.Nodos[nodo].CallTimeout("NodoRaft.AppendEntries", args, reply,
		50*time.Millisecond)
	if err != nil {
		return false
	} else {
		return true
	}
}

// Metodo Para() utilizado cuando no se necesita mas al nodo
//
// Quizas interesante desactivar la salida de depuracion
// de este nodo
func (nr *NodoRaft) para() {
	nr.quit <- true
	go func() { time.Sleep(5 * time.Millisecond); os.Exit(0) }()
}

// Devuelve "yo", mandato en curso y si este nodo cree ser lider
//
// Primer valor devuelto es el indice de este  nodo Raft el el conjunto de nodos
// la operacion si consigue comprometerse.
// El segundo valor es el mandato en curso
// El tercer valor es true si el nodo cree ser el lider
// Cuarto valor es el lider, es el indice del líder si no es él
func (nr *NodoRaft) obtenerEstado() (int, int, bool, int) {
	nr.Mux.Lock()
	defer nr.Mux.Unlock()
	var yo int = nr.Yo
	var mandato int = nr.currentTerm
	var esLider bool = (nr.estado == Leader)
	var idLider int = nr.IdLider
	return yo, mandato, esLider, idLider
}

// El servicio que utilice Raft (base de datos clave/valor, por ejemplo)
// Quiere buscar un acuerdo de posicion en registro para siguiente operacion
// solicitada por cliente.

// Si el nodo no es el lider, devolver falso
// Sino, comenzar la operacion de consenso sobre la operacion y devolver en
// cuanto se consiga
//
// No hay garantia que esta operacion consiga comprometerse en una entrada de
// de registro, dado que el lider puede fallar y la entrada ser reemplazada
// en el futuro.
// Primer valor devuelto es el indice del registro donde se va a colocar
// la operacion si consigue comprometerse.
// El segundo valor es el mandato en curso
// El tercer valor es true si el nodo cree ser el lide3r
// Cuarto valor es el lider, es el indice del líder si no es él
func (nr *NodoRaft) someterOperacion(operacion TipoOperacion) (int, int, bool,
	int, string) {
	var valorDevuelto string = ""

	// ---- tramo crítico corto ----
	nr.Mux.Lock()
	if nr.estado != Leader {
		term := nr.currentTerm
		idLider := nr.IdLider
		nr.Mux.Unlock()
		return -1, term, false, idLider, ""
	}

	// Añadir entrada al log
	indice := len(nr.log) + 1
	nr.log = append(nr.log, entrada{
		Term:      nr.currentTerm,
		Indice:    indice,
		Operacion: operacion,
	})

	// Actualizar índices locales del líder
	nr.matchIndex[nr.Yo] = len(nr.log)
	nr.nextIndex[nr.Yo] = len(nr.log) + 1

	term := nr.currentTerm
	nr.Mux.Unlock()
	// ---- fin tramo crítico corto ----

	// Replicar en seguidores (estas funciones ya hacen Lock internas)
	acks, mayoria := nr.replicarEntrada(indice)

	// Si alcanzamos mayoría, comprometemos p4
	comprometida := false

	if acks >= mayoria {
		nr.Mux.Lock()
		if indice > nr.commitIndex {
			nr.commitIndex = indice
		}
		
		//lectura p5
			//Si el indice es 5, necesitamos que el vector llegue al menos hasta la posición 6:
			for len(nr.canalesRespuesta) <= indice {
				nr.canalesRespuesta = append(nr.canalesRespuesta, nil)
			}

			//Creamos y guardamos el canal en la posición exacta del índice
			chRespuesta := make(chan string, 1)
			nr.canalesRespuesta[indice] = chRespuesta
			nr.Mux.Unlock()

			//Mandamos aplicar.
			nr.aplicadorOperaciones()

			//Esperamos respuesta del main.
			valorDevuelto = <-chRespuesta
		
		comprometida = true
	}

	if !comprometida {
		// No hay mayoría: para el cliente NO hay acuerdo
		return -1, term, true, nr.Yo, ""
	}else{
			return indice, term, true, nr.Yo, valorDevuelto
	}
}

func (nr *NodoRaft) replicarEnSeguidor(nodo int, hastaIndice int,
	acksChan chan bool) {
	success := nr.replicaSeguidor(nodo, hastaIndice)
	acksChan <- success
}

// Esperar respuestas con timeout
func (nr *NodoRaft) esperarReplicaciones(acks int, mayoria int,
	acksChan chan bool) int {
	timeout := time.After(500 * time.Millisecond)

	for i := 0; i < len(nr.Nodos)-1; i++ {
		select {
		case success := <-acksChan:
			if success {
				acks++
				if acks >= mayoria {
					return acks
				}
			}
		case <-timeout:
			return acks
		}
	}
	return acks
}

func (nr *NodoRaft) replicarEntrada(hastaIndice int) (int, int) {
	acks := 1 // Contamos nuestro propio log
	mayoria := len(nr.Nodos)/2 + 1
	acksChan := make(chan bool, len(nr.Nodos)-1)

	for i := range nr.Nodos {
		if i == nr.Yo {
			continue
		}
		go nr.replicarEnSeguidor(i, hastaIndice, acksChan)
	}

	return nr.esperarReplicaciones(acks, mayoria, acksChan), mayoria
}

/*
 * Auxiliar de someterOperacion:
 * Intenta que "nodo" tenga al menos la entrada "hastaIndice".
 * Devuelve true si el seguidor alcanza esa entrada.
 */
func (nr *NodoRaft) replicaSeguidor(nodo int, hastaIndice int) bool {
	for {
		// Devuelve true si ya está replicado
		if nr.matchIndex[nodo] >= hastaIndice {
			return true
		}

		args := nr.prepararArgsReplicacion(nodo)
		reply := Results{}

		if !nr.enviarLatido(nodo, &args, &reply) {
			return false
		}

		if !nr.procesarRespuestaReplicacion(&reply) {
			return false
		}

		if reply.Exito {
			nr.actualizarIndicesSeguidor(nodo, &args)
			if nr.matchIndex[nodo] >= hastaIndice {
				return true
			}
		} else {
			nr.retrocederIndice(nodo)
		}
	}
}

// Calcular PrevioLogIndice, PrevioLogTerm y las entradas pendientes.
func (nr *NodoRaft) prepararArgsReplicacion(nodo int) ArgAppendEntries {

	nr.Mux.Lock()
	indicePrevio := nr.nextIndex[nodo] - 1
	termPrevio := 0
	if indicePrevio > 0 && indicePrevio <= len(nr.log) {
		termPrevio = nr.log[indicePrevio-1].Term
	}

	entradas := append([]entrada(nil),
		nr.log[nr.nextIndex[nodo]-1:]...)

	nr.Mux.Unlock()

	return ArgAppendEntries{
		Term:            nr.currentTerm,
		IdLider:         nr.Yo,
		PrevioLogIndice: indicePrevio,
		PrevioLogTerm:   termPrevio,
		Entradas:        entradas,
		LeaderCommit:    nr.commitIndex,
	}
}

// Si el follower nos devuelve un término más grande, hemos dejado de ser líder.
func (nr *NodoRaft) procesarRespuestaReplicacion(reply *Results) bool {

	if reply.Term > nr.currentTerm {
		nr.actualizarTermino(reply.Term)
		return false
	} else {
		return nr.estado == Leader
	}
}

// Actualizamos nextIndex y el matchIndex de ese follower.
func (nr *NodoRaft) actualizarIndicesSeguidor(nodo int, args *ArgAppendEntries) {
	nr.matchIndex[nodo] = args.PrevioLogIndice + len(args.Entradas)
	nr.nextIndex[nodo] = nr.matchIndex[nodo] + 1
}

// Fallo de consistencia: retroceder nextIndex y reintentar.
func (nr *NodoRaft) retrocederIndice(nodo int) {
	if nr.nextIndex[nodo] > 1 {
		nr.nextIndex[nodo]--
	}
}

// -----------------------------------------------------------------------
// LLAMADAS RPC al API
//
// Si no tenemos argumentos o respuesta estructura vacia (tamaño cero)
type Vacio struct{}

func (nr *NodoRaft) ParaNodo(args Vacio, reply *Vacio) error {
	defer nr.para()
	return nil
}

type EstadoParcial struct {
	Mandato int
	EsLider bool
	IdLider int
}

type EstadoRemoto struct {
	IdNodo int
	EstadoParcial
}

func (nr *NodoRaft) ObtenerEstadoNodo(args Vacio, reply *EstadoRemoto) error {
	reply.IdNodo, reply.Mandato, reply.EsLider, reply.IdLider = nr.obtenerEstado()
	return nil
}

type ResultadoRemoto struct {
	ValorADevolver string
	IndiceRegistro int
	EstadoParcial
}

func (nr *NodoRaft) SometerOperacionRaft(operacion TipoOperacion,
	reply *ResultadoRemoto) error {
	reply.IndiceRegistro, reply.Mandato, reply.EsLider,
		reply.IdLider, reply.ValorADevolver = nr.someterOperacion(operacion)
	return nil
}

// -----------------------------------------------------------------------
// LLAMADAS RPC protocolo RAFT
//
// Structura de ejemplo de argumentos de RPC PedirVoto.
//
// Recordar
// -----------
// Nombres de campos deben comenzar con letra mayuscula !
type ArgsPeticionVoto struct {
	Term            int
	IdCandidato     int
	UltimoLogIndice int
	UltimoLogTerm   int
}

// Structura de ejemplo de respuesta de RPC PedirVoto,
//
// Recordar
// -----------
// Nombres de campos deben comenzar con letra mayuscula !
type RespuestaPeticionVoto struct {
	Term         int
	ConcederVoto bool
}

// Metodo para RPC PedirVoto
// Copiado prácticamente por completo de la teoría de Raft.
func (nr *NodoRaft) PedirVoto(peticion *ArgsPeticionVoto,
	reply *RespuestaPeticionVoto) error {
	nr.Mux.Lock()
	defer nr.Mux.Unlock()

	// No se le concede el voto al reply si term < currentTerm.
	if peticion.Term < nr.currentTerm {
		reply.Term = nr.currentTerm
		reply.ConcederVoto = false
		return nil
	} else if peticion.Term > nr.currentTerm {
		nr.currentTerm = peticion.Term
		nr.estado = Follower
		nr.votedFor = -1
		nr.enEleccion = false
	}

	reply.Term = peticion.Term
	reply.ConcederVoto = nr.debeConcederVoto(peticion)

	return nil
}

func (nr *NodoRaft) notificarLatido() {
	select {
	case nr.latidos <- true:
	default:
	}
}

func (nr *NodoRaft) debeConcederVoto(peticion *ArgsPeticionVoto) bool {
	votoValido := (nr.votedFor == -1 ||
		nr.votedFor == peticion.IdCandidato)
	logActualizado := nr.logEsActualizado(
		peticion.UltimoLogIndice, peticion.UltimoLogTerm)

	if votoValido && logActualizado {
		nr.votedFor = peticion.IdCandidato
		nr.notificarLatido()
		return true
	} else {
		return false
	}
}

/*
 * Auxiliar de PedirVoto():
 * Comprueba si el log del candidato está más actualizado que el nuestro.
 */
func (nr *NodoRaft) logEsActualizado(IndicePedido int, TermPedido int) bool {
	miIndice := len(nr.log)
	miTerm := 0
	if miIndice > 0 {
		miTerm = nr.log[miIndice-1].Term
	}

	if TermPedido != miTerm {
		// Si el nuevo term es mayor al que tengo yo, significa que su
		// información está más actualizada. Si es menor es información antigua.
		return TermPedido > miTerm
	} else {
		// Si está en el mismo term, si el índice pedido es mayor o igual al
		// nuestro, su información está más actualizada.
		return IndicePedido >= miIndice
	}
}

type ArgAppendEntries struct {
	Term            int
	IdLider         int
	PrevioLogIndice int
	PrevioLogTerm   int
	Entradas        []entrada
	LeaderCommit    int
}

type Results struct {
	Term  int
	Exito bool
}

// Metodo de tratamiento de llamadas RPC AppendEntries
// Copiado prácticamente por completo de la teoría de Raft.
func (nr *NodoRaft) AppendEntries(args *ArgAppendEntries,
	results *Results) error {

	nr.Mux.Lock()

	// valor por defecto.
	results.Term = nr.currentTerm
	results.Exito = false

	// Rechazar si el Term del líder es viejo.
	if args.Term < nr.currentTerm {
		nr.Mux.Unlock()
		return nil
	}

	nr.actualizarPorLider(args, results)
	nr.notificarLatido()

	if !nr.verificarConsistenciaLog(args) {
		nr.Mux.Unlock()
		return nil
	}

	nr.actualizarLog(args)
	cambio := nr.actualizarCommitIndex(args)

	results.Term = nr.currentTerm
	results.Exito = true

	nr.Mux.Unlock()
	if cambio {
		nr.aplicadorOperaciones() //p4
	}

	return nil
}

// Si el líder tiene término más nuevo, actualizamos.
func (nr *NodoRaft) actualizarPorLider(args *ArgAppendEntries,
	results *Results) {
	if args.Term > nr.currentTerm {
		nr.currentTerm = args.Term
		nr.estado = Follower
		nr.votedFor = -1
		nr.enEleccion = false
	}

	// Ya sabemos quién es el líder actual.
	nr.IdLider = args.IdLider
	nr.enEleccion = false

	nr.Logger.Printf("[Nodo %d] AppendEntries de %d: termLider=%d "+
		"miTermAntes=%d-> ahora term=%d; paso a Follower;"+
		" enEleccion=%v\n", nr.Yo, args.IdLider, args.Term,
		results.Term, nr.currentTerm, nr.enEleccion)

}

// Comprobamos consistencia del log previo.
func (nr *NodoRaft) verificarConsistenciaLog(args *ArgAppendEntries) bool {
	if args.PrevioLogIndice > 0 {
		// No hay suficiente log.
		if len(nr.log) < args.PrevioLogIndice {
			return false
		}
		// Tengo esa entrada, pero el Term no coincide, esto es conflictivo.
		if nr.log[args.PrevioLogIndice-1].Term != args.PrevioLogTerm {
			return false
		}
	}
	return true
}

// Insertar/sobreescribir entradas nuevas del líder.
func (nr *NodoRaft) actualizarLog(args *ArgAppendEntries) {
	if len(args.Entradas) > 0 {
		// Cortar mi log hasta PrevioLogIndice
		nr.log = nr.log[:args.PrevioLogIndice]
		// Añadir las entradas nuevas
		nr.log = append(nr.log, args.Entradas...)
	}
}

// Actualizar commitIndex siguiendo al líder.
func (nr *NodoRaft) actualizarCommitIndex(args *ArgAppendEntries) bool {
	anterior := nr.commitIndex
	if args.LeaderCommit > nr.commitIndex {
		if args.LeaderCommit > len(nr.log) {
			nr.commitIndex = len(nr.log)
		} else {
			nr.commitIndex = args.LeaderCommit
		}
	}
	return nr.commitIndex > anterior
}

// ----- Metodos/Funciones a utilizar como clientes
//
//

// Ejemplo de código enviarPeticionVoto
//
// nodo int -- indice del servidor destino en nr.nodos[]
//
// args *RequestVoteArgs -- argumentos par la llamada RPC
//
// reply *RequestVoteReply -- respuesta RPC
//
// Los tipos de argumentos y respuesta pasados a CallTimeout deben ser
// los mismos que los argumentos declarados en el metodo de tratamiento
// de la llamada (incluido si son punteros
//
// Si en la llamada RPC, la respuesta llega en un intervalo de tiempo,
// la funcion devuelve true, sino devuelve false
//
// la llamada RPC deberia tener un timout adecuado.
//
// Un resultado falso podria ser causado por una replica caida,
// un servidor vivo que no es alcanzable (por problemas de red ?),
// una petición perdida, o una respuesta perdida
//
// Para problemas con funcionamiento de RPC, comprobar que la primera letra
// del nombre  todo los campos de la estructura (y sus subestructuras)
// pasadas como parametros en las llamadas RPC es una mayuscula,
// Y que la estructura de recuperacion de resultado sea un puntero a estructura
// y no la estructura misma.
func (nr *NodoRaft) enviarPeticionVoto(nodo int, args *ArgsPeticionVoto,
	reply *RespuestaPeticionVoto) bool {
	error := nr.Nodos[nodo].CallTimeout("NodoRaft.PedirVoto",
		args, reply, 300*time.Millisecond)
	if error != nil {
		nr.Logger.Printf("Nodo %d: fallo RPC PeticionVoto a nodo %d\n",
			nr.Yo, nodo)
		return false
	} else {
		return true
	}
}

/*
 * Recorre las entradas comprometidas (commitIndex) y las envía, en orden,
 * a la máquina de estados por canalAplicarOperacion.
 * Se llama:
 * 		1. En el lider después de comprometer en someterOperacion.
 *		2. En los seguidores (y lider) cuando AppendEntries actualiza commitIndex.
 */
func (nr *NodoRaft) aplicadorOperaciones() { //p4

	for nr.lastApplied < nr.commitIndex {
		nr.lastApplied++
		indice := nr.lastApplied
		op := nr.log[indice-1].Operacion

		var ch chan string = nil
		//Verificamos si ya hay un canal esperando en esa posición.
		if indice < len(nr.canalesRespuesta) {
			ch = nr.canalesRespuesta[indice]
		}

		nr.canalAplicarOperacion <- AplicaOperacion{
			Indice:    indice,
			Operacion: op,
			Respuesta: ch, // Pasamos el canal encontrado (o nil)
		}
	}
}
