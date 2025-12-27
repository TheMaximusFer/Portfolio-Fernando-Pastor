package main

import (
    "bufio"
    "fmt"
    "strconv"
    "encoding/gob"
    "log"
    "net"
    "os"
    "practica1/com"
)

//Reutilizado de barrier.go
func readEndpoints(filename string) ([]string, error) {
        file, err := os.Open(filename)
        if err != nil {
                return nil, err
        }
        defer file.Close()

        var endpoints []string
        scanner := bufio.NewScanner(file)
        for scanner.Scan() {
                line := scanner.Text()
                if line != "" {
                        endpoints = append(endpoints, line)
                }
        }
        if err := scanner.Err(); err != nil {
                return nil, err
        }
        return endpoints, nil
}

//Pre:  min <= max
//Post: divide [min,max] en n bloques de intervalos iguales que serán 
//      enviados a procesar cada uno por un servidor .
func dividirIntervalo(min, max, n int) ([]com.TPInterval) {
	if n <= 0 { return nil }
	
        total := max - min + 1
	if total < n { n = total }
	
        tam_bloque := total / n
	resto := total % n

	bloques := make([]com.TPInterval, 0, n)
	puntero := min
	for i := 0; i < n; i++ {
		plus := 0
		if i < resto { plus = 1 }
		a := puntero
		b := puntero + tam_bloque + plus - 1
		bloques = append(bloques, com.TPInterval{Min: a, Max: b})
		puntero = b + 1
	}
	return bloques
}

//Pre:  -
//Post: envía una Request a un endpoint y devuelve la Reply
func procesarEnvioWorker(endpoint string, id int, interval com.TPInterval) (com.Reply, error) {
	conn, err := net.Dial("tcp", endpoint)
        if err != nil { return com.Reply{}, err }
	defer conn.Close()

	encoder := gob.NewEncoder(conn)
	decoder := gob.NewDecoder(conn)

	request := com.Request{Id: id, Interval: interval}
	if err := encoder.Encode(&request); err != nil {
		return com.Reply{}, err
	}
	var replyy com.Reply
	if err := decoder.Decode(&replyy); err != nil {
		return com.Reply{}, err
	}
	return replyy, nil
}


//Corta conexiones con worker y les manda señal de terminación
func sendStopAll(endpoints []string){
        for _, endpoint := range endpoints {
	        if conn, err := net.Dial("tcp", endpoint); err == nil {
			encoder := gob.NewEncoder(conn)
                        _ = encoder.Encode(com.Request{Id: -1})
			_ = conn.Close()
			fmt.Println("STOP enviado a ", endpoint)
		}
	}
}

func main() {

        if len(os.Args) != 3 && len(os.Args) != 4 {
                fmt.Println("Usage: go run master.go <endpoints_file> <min> <max>")
                fmt.Println("Usage: go run master.go <endpoints_file> stop")
                return
        }
        
	log.SetFlags(log.Lshortfile | log.Lmicroseconds)
        endpointsFile := os.Args[1]

        //endpoints tiene las IP de los workers
        endpoints, err := readEndpoints(endpointsFile);
        com.CheckError(err)
        cantidadEndpoints := len(endpoints)

        if cantidadEndpoints == 0 { 
                fmt.Println("No hay endpoints en el fichero:", endpointsFile)
                return
        }        
        if os.Args[2] == "stop" {
                sendStopAll(endpoints)
		return
	}

        //comprobamos que min y max sean valores correctos
        min, err := strconv.Atoi(os.Args[2])
        com.CheckError(err)
	max, err := strconv.Atoi(os.Args[3])
        com.CheckError(err)
        if min > max { 
                fmt.Println("Intervalo inválido") 
                return
        }

        //dividimos el intervalo en partes iguales a procesar posteriormente por cada worker
        bloques := dividirIntervalo(min, max, cantidadEndpoints)

        //Ponemos a trabajar a cada worker en su bloque del intervalo
        var resultado []int //Guarda todas las respuestas de los worker
        for i, endpoint := range endpoints {
                reply, err := procesarEnvioWorker(endpoint, i+1, bloques[i])
                if err != nil {
			fmt.Println("Error en ", endpoint, " -> ", err)
			continue
		}
                resultado = append(resultado, reply.Primes...) //Añadimos todos los prime de la respuesta al final de la lista de resultados
                fmt.Printf("Worker %s -> tarea %d en [%d, %d]: %d primos\n", endpoint, 
                           reply.Id, bloques[i].Min, bloques[i].Max, len(reply.Primes)) //Mostramos la termminación de un worker
        }

        fmt.Printf("Total de primos encontrados: %d\n", len(resultado))       

}

