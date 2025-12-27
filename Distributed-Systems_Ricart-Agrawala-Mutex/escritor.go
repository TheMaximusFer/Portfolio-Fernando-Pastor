/*
* AUTOR: Fernando Pastor Peralta, Javier Murillo Jimenez y Rafael Tolosana Calasanz
* ASIGNATURA: 30221 Sistemas Distribuidos del Grado en Ingeniería Informática
*                       Escuela de Ingeniería y Arquitectura - Universidad de Zaragoza
* FECHA: octubre 2025
* FICHERO: escritor.go
* DESCRIPCIÓN: Escritor
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

// Escribe al final del fichero
func EscribirFichero(fragmento string, filename string) error {
        file, err := os.OpenFile(filename, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
        CheckError(err)
        defer file.Close()

        _, err = file.WriteString(fragmento)
        return err
}

func main() {

    if len(os.Args) != 4 && len(os.Args) != 5 {
        fmt.Fprintf(os.Stderr, "Uso: escritor <id> <archivo_de_dispositivos> <archivo_escritura_lectura>\n")
        fmt.Fprintf(os.Stderr, "o\n")
        fmt.Fprintf(os.Stderr, "Uso: escritor <id> <archivo_de_dispositivos> <archivo_escritura_lectura> log\n")
        os.Exit(1)
    }

        id, err := strconv.Atoi(os.Args[1])
        archivo_de_dispositivos := os.Args[2]
        archivo_escritura_lectura := os.Args[3]
        CheckError(err) 

        escritor := ra.New(id, archivo_de_dispositivos, ra.OpWrite, archivo_escritura_lectura)

        if len(os.Args) == 5 && os.Args[4] == "log" { ra.EnableLog(true) } else { ra.EnableLog(false) }

        fmt.Printf("Escritor %d iniciado\n", id)
        fmt.Printf("Comandos:\n")
        fmt.Printf("  escribir <texto> - Escribir en el fichero\n")
        fmt.Printf("  estado           - Ver estado actual del fichero\n")
        fmt.Printf("  fin              - Terminar\n\n")

        time.Sleep(100 * time.Millisecond)

        scanner := bufio.NewScanner(os.Stdin)
        for {
                fmt.Printf("Escritor %d$ ", id)
                if !scanner.Scan() {
                        break
                }

                comando := scanner.Text()
                if comando == "" {
                        continue
                }
                if len(comando) >= 8 && comando[:8] == "escribir" {
                        texto := comando[8:]
                        if texto == "" {
                                fmt.Printf("Error: uso: escribir <texto>\n")
                                continue
                        }
                        texto += "\n"

                        fmt.Printf("Solicitando acceso exclusivo para escritura...\n")
                        escritor.PreProtocol()

                        // Sección escritura
                        fmt.Printf("EN SECCIÓN CRÍTICA (escritura)\n")
                        //time.Sleep(5 * time.Second)

                        err := EscribirFichero(texto, archivo_escritura_lectura)
                        CheckError(err)

                        //Esperamos a que todos los dispositivos actualicen su fichero exitosamente
                        escritor.ActualizarDispositivos(texto)

                        fmt.Printf("Escrito y propagado: %s", texto)
                        fmt.Printf("SALE DE SECCIÓN CRÍTICA\n\n")
                        escritor.PostProtocol()

                } else {
                        switch comando {
                        case "fin":
                                fmt.Printf("Escritor %d finalizando...\n", id)
                                escritor.Stop()
                                fmt.Printf("Escritor %d finalizado\n", id)
                                return

                        case "estado":
                                contenido, err := os.ReadFile(archivo_escritura_lectura)
                                CheckError(err)

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
                        }
                }
        }
        escritor.Stop()
}
