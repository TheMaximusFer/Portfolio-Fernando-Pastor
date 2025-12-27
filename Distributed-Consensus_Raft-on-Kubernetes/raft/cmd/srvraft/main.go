package main

import (
	//"errors"
	"fmt"
	"strings"
	//"log"
	"net"
	"net/rpc"
	"os"
	"raft/internal/comun/check"
	"raft/internal/comun/rpctimeout"
	"raft/internal/raft"
	"strconv"
	//"time"
)

func main() {
	const dns = "raft-service.default.svc.cluster.local:6000"
	const numNodos = 3 //si cambias las replicas del StatefulSet, cambiamos esto...
	const puerto = 6000

	//Esperamos que os.Args[1] sea el nombre correcto del pod, 
	// por ejemplo "raft-0".
	if len(os.Args) < 2 {
		check.CheckError(
			fmt.Errorf("uso: %s <nombre-pod>", os.Args[0]),
			"Main, argumentos insuficientes",
		)
	}

	//separamos nombre base ("raft") e índice ("0") del pod.
	partes := strings.Split(os.Args[1], "-")
	if len(partes) < 2 {
		check.CheckError(
			fmt.Errorf("nombre de pod %q no tiene formato <nombre>-<indice>", os.Args[1]),
			"Main, nombre de pod incorrecto",
		)
	}

	nombreBase := partes[0]

	me, err := strconv.Atoi(partes[len(partes)-1]) //me es el índice (número) de raft-<ínidice>
	check.CheckError(err, "Main, mal indice de nodo:")

	//Construir las direcciones de TODAS las réplicas.
	var direcciones []string
	for i := 0; i < numNodos; i++ {
		nodo := nombreBase + "-" + strconv.Itoa(i) + "." + dns
		// p.ej. "raft-0.raft-service.default.svc.cluster.local:6000"
		direcciones = append(direcciones, nodo)
	}

	var nodos []rpctimeout.HostPort
	// Resto de argumento son los end points como strings
	// De todas la replicas-> pasarlos a HostPort
	for _, endPoint := range direcciones {
		nodos = append(nodos, rpctimeout.HostPort(endPoint))
	}

	canalAplicar := make(chan raft.AplicaOperacion, 1000)
	maquinaEstados := make(map[string]string)
	go aplicarOperacion(maquinaEstados, canalAplicar)

	// Parte Servidor
	nr := raft.NuevoNodo(nodos, me, canalAplicar)
	rpc.Register(nr)

	//fmt.Println("Replica escucha en :", me, " de ", os.Args[2:])

	l, err := net.Listen("tcp", ":6000")
	check.CheckError(err, "Main listen error:")

	rpc.Accept(l)
}

func aplicarOperacion(maquinaEstados map[string]string, canal chan raft.AplicaOperacion) {
	for apl := range canal {
		op := apl.Operacion
		var resultado string
		switch op.Operacion {
		case "escribir":
			maquinaEstados[op.Clave] = op.Valor
			resultado = op.Valor
		case "leer":
			resultado = maquinaEstados[op.Clave]
		}

		//Si nos pasaron un canal, contestamos (p5).
		if apl.Respuesta != nil {
			apl.Respuesta <- resultado
		}

	}
}
