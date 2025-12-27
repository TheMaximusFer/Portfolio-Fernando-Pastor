package main

import (
	"encoding/gob"
	"log"
	"net"
	"os"
	"practica1/com"
)

const NWORKERS = 5

// PRE: -
// POST: IsPrime devuelve verdad si n es primo y falso en caso contrario
func IsPrime(n int) bool {
    if n < 2 {
        return false
    }
    if n%2 == 0 {
        return n == 2
    }
    for i := 3; i <= n/i; i += 2 {
        if n%i == 0 {
            return false
        }
    }
    return true
}

// PRE: interval.A < interval.B
// POST: FindPrimes devuelve todos los números primos comprendidos en el
//	intervalo [interval.A, interval.B]
func findPrimes(interval com.TPInterval) (primes []int) {
	for i := interval.Min; i <= interval.Max; i++ {
		if IsPrime(i) {
			primes = append(primes, i)
		}
	}
	return primes
}

func processRequest(conn net.Conn) (reply com.Reply, end bool) {
	defer conn.Close()

	var request com.Request
	decoder := gob.NewDecoder(conn)
	if err := decoder.Decode(&request); err != nil {
		log.Println("Error decodificando Request:", err)
		return com.Reply{}, false
	}

	if request.Id == -1 {
		log.Println("Petición de fin recibida. Cerrando conexión")
		return com.Reply{}, true
	}

	primes := findPrimes(request.Interval)
	reply = com.Reply{Id: request.Id, Primes: primes}

	encoder := gob.NewEncoder(conn)
	if err := encoder.Encode(&reply); err != nil {
		log.Println("Error codificando Reply:", err)
		return com.Reply{}, false
	}

	return reply, false
}

func worker(tasks <-chan net.Conn, results chan<- com.Reply, done chan<- bool, workerDone chan<- bool) {
	defer func() { workerDone <- true }()
	for conn := range tasks {
		reply, end := processRequest(conn)
		if end {
			done <- true
			continue
		}
		results <- reply
	}
}

func resultHandler(results <-chan com.Reply) {
	for reply := range results {
		log.Println("Resultado listo para la petición", reply.Id)
	}
}

func main() {
	args := os.Args
	if len(args) != 2 {
		log.Println("Error: endpoint missing: go run server.go ip:port")
		os.Exit(1)
	}

	endpoint := args[1]
	listener, err := net.Listen("tcp", endpoint)
	com.CheckError(err)

	log.SetFlags(log.Lshortfile | log.Lmicroseconds)
	log.Println("***** Listening for new connection in endpoint ", endpoint)

	tasks := make(chan net.Conn)
	results := make(chan com.Reply)
	done := make(chan bool)
	stop := make(chan bool)
	workerDone := make(chan bool)

	for i := 0; i < NWORKERS; i++ {
		go worker(tasks, results, done, workerDone)
	}

	go resultHandler(results)

	//Bucle de aceptación de conexiones
	go func() {
		defer func() { stop <- true }()
		for {
			conn, err := listener.Accept()
			if err != nil {
				log.Println("Accept detenido:", err)
				close(tasks) // Cerramos tasks para terminar los workers
				return
			}
			tasks <- conn
		}
	}()

	<-done							//Esperar -1 para parar
	log.Println("Señal de fin recibida. Parando servidor…")
	_ = listener.Close()			//Cerramos el listener (provoca err en func de aceptación de conexiones)
	<-stop 							//Esperamos a que termine el acceptor
	for i := 0; i < NWORKERS; i++ { //Cerramos los workers
		<-workerDone
	}
	close(results) //Cerramos results para que termine resultHandler

	log.Println("Servidor detenido correctamente.")

}
