package main

import (
	"fmt"
	"os"
	"time"
	"raft/internal/comun/rpctimeout"
	"raft/internal/raft"
	cltraft "raft/pkg/pkgclt"
)

//Raft puede devolver -1 si no se ha podido someter la operación por lo que insistimos en someter la operación hasta que se comprometa.
func someterConReintentos(cli *cltraft.ClienteRaft, op raft.TipoOperacion) (cltraft.ResultadoOperacion, error) {
	intentos := 0
	maxIntentos := 10 //Reintentar durante unos 20 segundos (10 * 2s).
	
	for {
		intentos++
		res, err := cli.SometerOperacion(op)
		
		//Si no hay error, es que un líder nos ha contestado.
		if err == nil {
			return res, nil
		}
		//Si falló, es probable que estén votando. Esperamos.
		fmt.Printf("   ... (Intento %d/%d) Sin líder aún... Esperando elección...\n", intentos, maxIntentos)
		
		if intentos >= maxIntentos {
			return cltraft.ResultadoOperacion{}, err
		}
		
		time.Sleep(2 * time.Second)
	}
}

//Función auxiliar para hacer Escritura + Lectura + Verificación.
func testOperacion(cli *cltraft.ClienteRaft, numPaso int, clave string, valor string) {

	//ESCRITURA.
	opEscribir := raft.TipoOperacion{
		Operacion: "escribir",
		Clave: clave,
		Valor: valor,
	}

	resWrite, err := someterConReintentos(cli, opEscribir)
	if err != nil {
		fmt.Printf("  Error al escribir clave %s: %v\n", clave, err)
		os.Exit(1)
	}else{
		fmt.Printf("	-> Escritura OK: idx=%d, mandato=%d, lider=%d\n", 
					resWrite.IndiceRegistro, resWrite.Mandato, resWrite.IdLider)
	}

	//LECTURA.
	opLeer := raft.TipoOperacion{
		Operacion: "leer",
		Clave: clave,
		Valor: "",
	}

	resRead, err := someterConReintentos(cli, opLeer)
	if err != nil {
		fmt.Printf("  Error al leer clave %s: %v\n", clave, err)
		os.Exit(1)
	}

	//COMPARACIÓN.
	if resRead.ValorADevolver == valor {
		fmt.Printf("	-> Lectura OK: '%s' == '%s' (Esperado)\n", resRead.ValorADevolver, valor)
	} else {
		fmt.Printf("  AVISO Lectura: Recibido '%s' (Esperaba '%s')\n", resRead.ValorADevolver, valor)
		os.Exit(1)
	}
}


func main() {
	//Direcciones de los nodos Raft.
	direcciones := []string{
    "raft-0.raft-service.default.svc.cluster.local:6000",
    "raft-1.raft-service.default.svc.cluster.local:6000",
    "raft-2.raft-service.default.svc.cluster.local:6000",
	}

	var nodos []rpctimeout.HostPort
	for _, d := range direcciones {
		nodos = append(nodos, rpctimeout.HostPort(d))
	}

	cli := cltraft.NuevoCliente(nodos)
	
	fmt.Println("--- INICIO DE PRUEBAS DE CLIENTE ---")

	testOperacion(cli, 1, "k1", "valor_inicial_1")
	testOperacion(cli, 2, "k2", "valor_inicial_2")
	testOperacion(cli, 3, "k3", "valor_inicial_3")

	fmt.Println("\nPRUEBA DE SOBRESCRITURA")
	testOperacion(cli, 4, "k1", "VALOR_ACTUALIZADO")

	fmt.Println("\n--- TODAS LAS PRUEBAS COMPLETADAS CON ÉXITO ---")
}
