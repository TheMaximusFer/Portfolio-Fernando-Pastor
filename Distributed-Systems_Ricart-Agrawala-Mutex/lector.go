/*
* AUTOR: Fernando Pastor Peralta, Javier Murillo Jimenez y Rafael Tolosana Calasanz
* ASIGNATURA: 30221 Sistemas Distribuidos del Grado en Ingeniería Informática
*                       Escuela de Ingeniería y Arquitectura - Universidad de Zaragoza
* FECHA: octubre 2025
* FICHERO: lector.go
* DESCRIPCIÓN: Lector
*/
package main

import (
        "bufio"
        "fmt"
        "os"
        "practica2/ra"
        "strconv"
        "time"
)

func CheckError(err error) {
        if err != nil {
                fmt.Fprintf(os.Stderr, "Fatal error: %s", err.Error())
                os.Exit(1)
        }
}

// Lee el fichero compartido
func LeerFichero(filename string) string {
        contenido, err := os.ReadFile(filename)
        CheckError(err)
        return string(contenido)
}

func main() {
        if len(os.Args) != 4 && len(os.Args) != 5 {
                fmt.Fprintf(os.Stderr, "Uso: lector <id> <archivo_de_dispositivos> <archivo_escritura_lectura>\n")
                fmt.Fprintf(os.Stderr, "o\n")
                fmt.Fprintf(os.Stderr, "Uso: lector <id> <archivo_de_dispositivos> <archivo_escritura_lectura> log\n")
                os.Exit(1)
        }

        id, err := strconv.Atoi(os.Args[1])
        archivo_de_dispositivos := os.Args[2]
        archivo_escritura_lectura := os.Args[3]
        CheckError(err)

        lector := ra.New(id, archivo_de_dispositivos, ra.OpRead, archivo_escritura_lectura)

        if len(os.Args) == 5 && os.Args[4] == "log" { ra.EnableLog(true) } else { ra.EnableLog(false) }

        fmt.Printf("Lector %d iniciado\n", id)
        fmt.Printf("Copia local: %s\n", archivo_escritura_lectura)
        fmt.Printf("Comandos:\n")
        fmt.Printf("  leer    - Leer mi copia local del fichero\n")
        fmt.Printf("  estado  - Ver estado de mi copia local\n")
        fmt.Printf("  fin     - Terminar\n\n")

        time.Sleep(100 * time.Millisecond)

        scanner := bufio.NewScanner(os.Stdin)

        for {
                fmt.Printf("Lector %d$ ", id)
                if !scanner.Scan() {
                        break
                }

                comando := scanner.Text()
                if comando == "" {
                        continue
                }

                switch comando {
                case "fin":
                        fmt.Printf("Lector %d finalizando...\n", id)
                        lector.Stop()
                        fmt.Printf("Lector %d finalizado\n", id)
                        return

                case "leer":
                        fmt.Printf("Solicitando acceso para lectura...\n")
                        lector.PreProtocol()

                        // Inicio de la sección crítica
                        fmt.Printf("En sección crítica (lectura)\n")
                        //time.Sleep(5 * time.Second)

                        contenido := LeerFichero(archivo_escritura_lectura)
                        fmt.Printf("Contenido (copia %d):\n%s\n", id, contenido)
                        fmt.Printf("Saliendo de la sección crítica\n")

                        // Liberando el acceso a la sección crítica
                        lector.PostProtocol()

                case "estado":
                        contenido := LeerFichero(archivo_escritura_lectura)
                        lineas := 0
                        for i := 0; i < len(contenido); i++ {
                                if contenido[i] == '\n' {
                                        lineas++
                                }
                        }

                        if len(contenido) > 0 && contenido[len(contenido)-1] != '\n' {
                                lineas++
                        }

                        fmt.Printf("Estado copia local: %d líneas:\n%s\n", lineas, contenido)


                default:
                        fmt.Printf("Comando desconocido. Usa: leer, estado, fin\n")
                }
        }
        lector.Stop()
}
