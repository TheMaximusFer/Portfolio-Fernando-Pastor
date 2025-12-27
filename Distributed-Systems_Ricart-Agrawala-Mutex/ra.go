/*
* AUTOR: Fernando Pastor Peralta, Javier Murillo Jimenez y Rafael Tolosana Calasanz
* ASIGNATURA: 30221 Sistemas Distribuidos del Grado en Ingeniería Informática
*                       Escuela de Ingeniería y Arquitectura - Universidad de Zaragoza
* FECHA: octubre 2025
* FICHERO: ra.go
* DESCRIPCIÓN: Implementación del algoritmo de Ricart-Agrawala Generalizado en Go
*/
package ra

import (
        "bufio"
        "os"
        "sync"
        "practica2/ms"
        "log"
        "strings"
        "io"
)

const OpRead int = 0  //operación leer
const OpWrite int = 1 //operación escribir

type Request struct {
        Clock int //reloj lógico del proceso que envia el mensaje
        Pid   int //id del proceso que envia el mensaje
        Op    int //operación (leer o escribir)
}

type Reply struct{}

type Update struct {
    Fragment string
        Pid      int //id del proceso que envia el mensaje
}

type UpdateACK struct{}



//Estructura de datos compartida por los procesos.
type RASharedDB struct {
    OurSeqNum   int                         // número de secuencia del proceso   
    HighSeqNum  int                             // número de secuencia más alto visto
    OutRepCnt   int                                 // número de replies que faltan para entrar a la SC
    ReqCS       bool                        // indica si el proceso está pidiendo acceso a la SC
    RepDefd     []bool                      // para cada proceso, si se le debe un reply
    MS          ms.MessageSystem    // sistema de mensajes
    done        chan bool                   // para terminar los procesos
    chrep       chan bool           // canal para recibir los replies
    Mutex       sync.Mutex                  // mutex para proteger concurrencia sobre las variables
    
    id          int                         // id del proceso
    MyOp        int                         // operación actual (leer o escribir)
    N           int                         // número de procesos
    exclude     [2][2]bool                  // matriz de exclusión mutua entre operaciones de lectura y escritura
        chUpdateACK chan bool                   // canal para recibir ACKs de los dispositivos para confirmar que han actualizado su fichero
        archivo_EscLec string                   // fichero en el que escribir o leer
}


//Crea el objeto RASharedDB e inicia el receptor de mensajes.
func New(me int, usersFile string, Op int, archivo_EscLec string) (*RASharedDB) {
        log.SetFlags(log.Lshortfile | log.Lmicroseconds)
    messageTypes := []ms.Message{Request{}, Reply{}, Update{}, UpdateACK{}}
    n := CountLines(usersFile)
    msgs := ms.New(me, usersFile, messageTypes)
    ra := &RASharedDB{
                OurSeqNum:  0,
                HighSeqNum: 0,
                OutRepCnt:  0,
                ReqCS:      false,
                RepDefd:    make([]bool, n+1),
                MS:         msgs,
                done:       make(chan bool, 1),
                chrep:      make(chan bool, n),
        Mutex:      sync.Mutex{},
                id:         me,
                MyOp:       Op,
                N:          n,
                exclude: [2][2]bool{
                      /*read*/ /*write*/
                         /*read*/ {false,    true},
                        /*write*/ {true,     true},
                },
                chUpdateACK: make(chan bool, n),
                archivo_EscLec: archivo_EscLec,
        }

        go ra.receiver()
    return ra
}

//Pre: Verdad
//Post: Realiza  el  PreProtocol  para el  algoritmo de
//      Ricart-Agrawala Generalizado
func (ra *RASharedDB) PreProtocol(){
        // 1) Petición de entrada a la SC:
        ra.Mutex.Lock()
    ra.ReqCS = true
        ra.HighSeqNum++
        ra.OurSeqNum = ra.HighSeqNum
        ra.Mutex.Unlock()
    ra.OutRepCnt = ra.N - 1

    localClock := ra.OurSeqNum
        localOp := ra.MyOp
        me := ra.id

        logger.Printf("REQ pid=%d clk=%d op=%d", ra.id, localClock, localOp)

        // 2) Broadcast Request(me, localClock, Op) a todos menos a mí
    req := Request{Clock: localClock, Pid: me, Op: localOp}
        for pid := 1; pid <= ra.N; pid++ {
                if pid == me {
                        continue
                }
                        ra.MS.Send(pid, req)
        }

        // 3) Esperar N-1 Reply
        for ra.OutRepCnt > 0 {
        <-ra.chrep
        ra.OutRepCnt--
        }

        logger.Printf("ENTER SC pid=%d op=%d", ra.id, ra.MyOp)
}

//Pre: Verdad
//Post: Realiza  el  PostProtocol  para el  algoritmo de
//      Ricart-Agrawala Generalizado
func (ra *RASharedDB) PostProtocol(){
        logger.Printf("EXIT SC pid=%d op=%d", ra.id, ra.MyOp)
        // 1) Salimos de SC, ya no la queremos
        ra.Mutex.Lock()
        ra.ReqCS = false

        // 2) Copiar y limpiar diferidos bajo mutex
        deferList := make([]int, 0, ra.N)
        for pid := 1; pid <= ra.N; pid++ {
                if ra.RepDefd[pid] {
                        deferList = append(deferList, pid)
                        ra.RepDefd[pid] = false
                }
        }
        ra.Mutex.Unlock()

        // 3) Responder ahora a los diferidos
        for _, pid := range deferList {
                ra.MS.Send(pid, Reply{})
        }
}

func (ra *RASharedDB) Stop() {
    select {
    case ra.done <- true:
    default:
    }

    ra.MS.Stop()
}


//Pre: Verdad
//Post: Actualiza el fichero de lectura-escritura local añadiendo 
//              "texto" y espera a los UpdateACK de los demás dispositivos 
//              para confirmar que han actualizado su fichero, antes de salir 
//              de la zona de exclusión mutua.
func (ra *RASharedDB) ActualizarDispositivos(texto string){

        //Propagamos el update a todos los dispositivos
        for pid := 1; pid <= ra.N; pid++ {
                if pid == ra.id { //Nos actualizamos a nosotros mismos en escritor
                        logger.Printf("UPDATE->%d len=%d", ra.id, len(texto))
                        continue

                }
                ra.MS.Send(pid, Update{Fragment: texto, Pid: ra.id})
                logger.Printf("UPDATE->%d len=%d", pid, len(texto))
        }

        //Esperamos la respuesta de que han actualizado todos
        faltantes := ra.N - 1 
        for faltantes > 0 {
        <-ra.chUpdateACK
                logger.Printf("ACK recibido")
        faltantes--
        }
}



//Recibe mensajes y los procesa dependiendo de si son Request o Reply. Si llega done, termina.
func (ra *RASharedDB) receiver() {
        for {
                select {
                case <-ra.done:
                        return
                default:
                        msg := ra.MS.Receive()
                        if msg == nil {
                                return
                        }
                        switch m := msg.(type) {
                        case Request:
                                ra.handleRequest(m)
                        case Reply:
                                ra.chrep <- true
                        case Update:
                        ra.handleUpdate(m)
                        case UpdateACK:
                                ra.chUpdateACK <- true
                        }
                }
        }
}


//Función que maneja la recepción de un mensaje Request.
func (ra *RASharedDB) handleRequest(req Request){
    ra.Mutex.Lock()

        //mantener reloj máximo
        if req.Clock > ra.HighSeqNum {
                ra.HighSeqNum = req.Clock
        }

    deferIt := ra.ReqCS && ra.exclude[ra.MyOp][req.Op] && (req.Clock > ra.OurSeqNum || (ra.OurSeqNum == req.Clock && ra.id < req.Pid)) 

        if deferIt { //Se difiere la respuesta
                ra.RepDefd[req.Pid] = true
                logger.Printf("DEFER to=%d (req clk=%d)", req.Pid, req.Clock)
                ra.Mutex.Unlock()
                return
        }else{       //Se responde inmediatamente
                logger.Printf("REPLY to=%d", req.Pid)
        ra.Mutex.Unlock()
        ra.MS.Send(req.Pid, Reply{})
        return
    }
}


//Función que maneja la recepción de un mensaje Update.
func (ra *RASharedDB) handleUpdate(update Update){
    ra.Mutex.Lock()
    defer ra.Mutex.Unlock()

    file, err := os.OpenFile(ra.archivo_EscLec, os.O_APPEND|os.O_WRONLY|os.O_CREATE, 0644)
    if err != nil {
        log.Printf("Error abriendo %s: %v\n", ra.archivo_EscLec, err)
        return
    }
    defer file.Close()

    _, err = file.WriteString(update.Fragment)
    if err != nil {
        log.Printf("Error escribiendo en %s: %v\n", ra.archivo_EscLec, err)
    }

        //Comunicamos al proceso que ha actualizado que nosotros lo hemos hecho ya.
        logger.Printf("UPDATE from=%d; sending ACK; len=%d", update.Pid, len(update.Fragment))
        ra.MS.Send(update.Pid, UpdateACK{}) 
}


//Utilidad para contar líneas en un fichero.
func CountLines(usersFile string) int {
    file, err := os.Open(usersFile)
    if err != nil {
        return 0
    }
    defer file.Close()

    scanner := bufio.NewScanner(file)
    count := 0
    for scanner.Scan() {
    if len(strings.TrimSpace(scanner.Text())) > 0 {
        count++
    }
        }
    return count
}


//LOG
var logger = log.New(io.Discard, "", log.Lshortfile|log.Lmicroseconds)

func EnableLog(v bool) {
    if v { logger.SetOutput(os.Stderr) } else { logger.SetOutput(io.Discard) }
}

