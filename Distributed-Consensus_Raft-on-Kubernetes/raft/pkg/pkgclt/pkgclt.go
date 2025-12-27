package pkgclt

import (
	"errors"
	"time"

	"raft/internal/comun/rpctimeout"
	raft "raft/internal/raft"
)

type ClienteRaft struct {
	nodos       []rpctimeout.HostPort
	liderActual int
}

type EstadoNodo struct {
	IdNodo  int  // índice de esa réplica en el array global
	Mandato int  // currentTerm
	EsLider bool // true si esa réplica se cree líder
	IdLider int  // a quién cree que es el líder (-1 si no sabe)
}
type ResultadoOperacion struct {
	IndiceRegistro       int    // posición en el log donde se metió
	Mandato              int    // mandato en que se aceptó
	IdLider              int    // qué réplica actuó / cree ser líder
	EsLiderQuienResponde bool   // true si quien respondió era el líder
	ValorADevolver       string // para lecturas (aún vacío en P3)
}

func NuevoCliente(nodos []rpctimeout.HostPort) *ClienteRaft {
	return &ClienteRaft{
		nodos:       nodos,
		liderActual: -1,
	}
}

func (c *ClienteRaft) NumNodos() int {
	return len(c.nodos)
}

func (c *ClienteRaft) LiderCreido() int {
	return c.liderActual
}

/*
 * Hace NodoRaft.ObtenerEstadoNodo
 * Devuelve el estado y si la RPC ha contestado sin error.
 */
func (c *ClienteRaft) ObtenerEstadoNodo(id int) (EstadoNodo, bool) {
	var reply raft.EstadoRemoto
	error := c.nodos[id].CallTimeout("NodoRaft.ObtenerEstadoNodo", raft.Vacio{}, &reply, 150*time.Millisecond)

	if error != nil {
		return EstadoNodo{}, false
	}

	return EstadoNodo{
		IdNodo:  reply.IdNodo,
		Mandato: reply.Mandato,
		EsLider: reply.EsLider,
		IdLider: reply.IdLider,
	}, true
}

/*
 * Hace NodoRaft.PararNodo
 * Devuelve si la RPC ha contestado sin error.
 */
func (c *ClienteRaft) PararNodo(id int) bool {
	var vacio raft.Vacio
	error := c.nodos[id].CallTimeout("NodoRaft.ParaNodo", raft.Vacio{}, &vacio, 150*time.Millisecond)
	return error == nil
}

/*
 * Intenta someter la operación op a un líder Raft.
 * Si tiene éxito, devuelve el resultado y nil error.
 */
func (c *ClienteRaft) SometerOperacion(op raft.TipoOperacion) (ResultadoOperacion, error) {

	visitado := make(map[int]bool)
	var candidatos []int

	// Primero, si creemos tener líder válido.
	if c.liderActual >= 0 && c.liderActual < len(c.nodos) {
		candidatos = append(candidatos, c.liderActual)
	}

	// Ponemos todos los nodos como candidatos, excepto el líder.
	for i := 0; i < len(c.nodos); i++ {
		if c.liderActual == i {
			continue
		}
		candidatos = append(candidatos, i)
	}

	// Vamos probando candidatos en orden.
	// Si un nodo nos dice que el lider es otro, metemos es otro al final de la lista (si no lo hemos probado).
	for pos := 0; pos < len(candidatos); pos++ {
		id := candidatos[pos]
		if visitado[id] {
			continue
		}
		visitado[id] = true

		res, ok := c.rpcSometerOperacion(id, op)
		if !ok {
			// este nodo ha respondió con error, pasamos al siguiente
			continue
		}

		// Si este nodo se declara líder, return.
		if res.EsLiderQuienResponde {
			return res, nil
		}

		// No es líder. Si nos dice quién es el líder, lo añadimos a la lista de candidatos.
		if res.IdLider >= 0 &&
			res.IdLider < len(c.nodos) &&
			!visitado[res.IdLider] {

			// Añadimos ese posible líder al final para intentar luego.
			candidatos = append(candidatos, res.IdLider)
		}
	}

	return ResultadoOperacion{}, errors.New("no se pudo contactar con un líder Raft o no se obtuvo confirmación de la operación")
}

/*
 * Auxiliar de SometerOperacion:
 * Hace NodoRaft.SometerOperacionRaft
 * Devuelve el estado y si la RPC ha contestado sin error.
 */
func (c *ClienteRaft) rpcSometerOperacion(id int, op raft.TipoOperacion) (ResultadoOperacion, bool) {

	var reply raft.ResultadoRemoto

	error := c.nodos[id].CallTimeout("NodoRaft.SometerOperacionRaft", op, &reply, 5000*time.Millisecond)

	if error != nil {
		return ResultadoOperacion{}, false
	}

	// Actualizamos líder con lo que dice el servidor.
	if reply.IdLider >= 0 && reply.IdLider < len(c.nodos) {
		c.liderActual = reply.IdLider
	}

	return ResultadoOperacion{
		IndiceRegistro:       reply.IndiceRegistro,
		Mandato:              reply.Mandato,
		IdLider:              reply.IdLider,
		EsLiderQuienResponde: reply.EsLider,
		ValorADevolver:       reply.ValorADevolver,
	}, true
}
