/****************************************************************************/
/* Plantilla para implementación de funciones del cliente (rcftpclient)     */
/* $Revision$ */
/* Aunque se permite la modificación de cualquier parte del código, se */
/* recomienda modificar solamente este fichero y su fichero de cabeceras asociado. */
/****************************************************************************/

/**************************************************************************/
/* INCLUDES                                                               */
/**************************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <math.h>
#include "rcftp.h" // Protocolo RCFTP
#include "rcftpclient.h" // Funciones ya implementadas
#include "multialarm.h" // Gestión de timeouts
#include "vemision.h" // Gestión de ventana de emisión
#include "misfunciones.h"

/**************************************************************************/
/* VARIABLES GLOBALES                                                     */
/**************************************************************************/
// elegir 1 o 2 autores y sustituir "Apellidos, Nombre" manteniendo el formato
char* autores="Autor: Duarte Jauregui, Fabrizio\nAutor: Pastor Peralta, Fernando"; 
//char* autores="Autor: Apellidos, Nombre\nAutor: Apellidos, Nombre" // dos autores

// variable para indicar si mostrar información extra durante la ejecución
// como la mayoría de las funciones necesitaran consultarla, la definimos global
extern char verb;


// variable externa que muestra el número de timeouts vencidos
// Uso: Comparar con otra variable inicializada a 0; si son distintas, tratar un timeout e incrementar en uno la otra variable
extern volatile const int timeouts_vencidos;


//PARA USO DE bool
#define TRUE  1
#define FALSE 0

/**************************************************************************/
/* Obtiene la estructura de direcciones del servidor */
/**************************************************************************/
struct addrinfo* obtener_struct_direccion(char *dir_servidor, char *servicio, char f_verbose){
 struct addrinfo hints,     // Variable para especificar la solicitud
                    *servinfo, // Puntero para respuesta de getaddrinfo()
                    *direccion;// Puntero para recorrer la lista de
                               // direcciones de servinfo
    int status;     // Finalización correcta o no de la llamada getaddrinfo()
    int numdir = 1; // Contador de estructuras de direcciones en la
                    // lista de direcciones de servinfo

    // sobreescribimos con ceros la estructura
    // para borrar cualquier dato que pueda malinterpretarse
    memset(&hints, 0, sizeof hints);

    // genera una estructura de dirección con especificaciones de la solicitud
    if (f_verbose)
    {
        printf("1 - Especificando detalles de la estructura de direcciones a solicitar... \n");
        fflush(stdout);
    }

    // especificamos la familia de direcciones con la que queremos trabajar:
    // AF_UNSPEC, AF_INET (IPv4), AF_INET6 (IPv6), etc.
    hints.ai_family = AF_UNSPEC;                                //----------------------4

    if (f_verbose)
    {
        printf("\tFamilia de direcciones/protocolos: ");
        switch (hints.ai_family)
        {
            case AF_UNSPEC: printf("IPv4 e IPv6\n"); break;
            case AF_INET:   printf("IPv4\n"); break;
            case AF_INET6:  printf("IPv6\n"); break;
            default:        printf("No IP (%d)\n", hints.ai_family); break;
        }
        fflush(stdout);
    }

    // especificamos el tipo de socket deseado:
    // SOCK_STREAM (TCP), SOCK_DGRAM (UDP), etc.
    hints.ai_socktype = SOCK_DGRAM;                            //----------------------5

    if (f_verbose)
    {
        printf("\tTipo de comunicación: ");
        switch (hints.ai_socktype)
        {
            case SOCK_STREAM: printf("flujo (TCP)\n"); break;
            case SOCK_DGRAM:  printf("datagrama (UDP)\n"); break;
            default:          printf("no convencional (%d)\n", hints.ai_socktype); break;
        }
        fflush(stdout);
    }

    // flags específicos dependiendo de si queremos la dirección como cliente
    // o como servidor
    if (dir_servidor != NULL)
    {
        // si hemos especificado dir_servidor, es que somos el cliente
        // y vamos a conectarnos con dir_servidor
        if (f_verbose) printf("\tNombre/dirección del equipo: %s\n", dir_servidor);
    }
    else
    {
        // si no hemos especificado, es que vamos a ser el servidor
        if (f_verbose) printf("\tNombre/dirección: equipo local\n");

        // especificar flag para que la IP se rellene con lo necesario para hacer bind
        // consultar documentación con: 'man getaddrinfo')
        hints.ai_flags = AI_PASSIVE;                            //----------------------6
    }
    if (f_verbose) printf("\tServicio/puerto: %s\n", servicio);

    // llamada getaddrinfo() para obtener la estructura de direcciones solicitada
    // getaddrinfo() pide memoria dinámica al SO,
    // la rellena con la estructura de direcciones
    // y escribe en servinfo la dirección donde se encuentra dicha estructura.
    // La memoria dinámica reservada en una función NO se libera al salir de ella
    // Para liberar esta memoria, usar freeaddrinfo()
    if (f_verbose)
    {
        printf("2 - Solicitando la estructura de direcciones con getaddrinfo()... ");
        fflush(stdout);
    }
    status = getaddrinfo(dir_servidor, servicio, &hints, &servinfo);
    if (status != 0)
    {
        fprintf(stderr,"Error en la llamada getaddrinfo(): %s\n", gai_strerror(status));
        exit(1);
    }
    if (f_verbose) printf("hecho\n");

    // imprime la estructura de direcciones devuelta por getaddrinfo()
    if (f_verbose)
    {
        printf("3 - Analizando estructura de direcciones devuelta... \n");
        direccion = servinfo;
        while (direccion != NULL)
        {   // bucle que recorre la lista de direcciones
            printf("    Dirección %d:\n", numdir);
            printsockaddr((struct sockaddr_storage*) direccion->ai_addr);
            // "avanzamos" a la siguiente estructura de direccion
            direccion = direccion->ai_next;
            numdir++;
        }
    }

    // devuelve la estructura de direcciones devuelta por getaddrinfo()
    return servinfo;
}
/**************************************************************************/
/* Imprime una direccion */
/**************************************************************************/
void printsockaddr(struct sockaddr_storage * saddr) {
struct sockaddr_in *saddr_ipv4; // puntero a estructura de dirección IPv4
    // el compilador interpretará lo apuntado como estructura de dirección IPv4
    struct sockaddr_in6 *saddr_ipv6; // puntero a estructura de dirección IPv6
    // el compilador interpretará lo apuntado como estructura de dirección IPv6
    void *addr; // puntero a dirección
    // como puede ser tipo IPv4 o IPv6 no queremos que el compilador la
    // interprete de alguna forma particular, por eso void
    char ipstr[INET6_ADDRSTRLEN]; // string para la dirección en formato texto
    int port; // almacena el número de puerto al analizar estructura devuelta

    if (saddr == NULL)
    {
        printf("La dirección está vacía\n");
    }
    else
    {
        printf("\tFamilia de direcciones: ");
        fflush(stdout);
        if (saddr->ss_family == AF_INET6)
        {   // IPv6
            printf("IPv6\n");
            // apuntamos a la estructura con saddr_ipv6 (cast evita warning),
            // así podemos acceder al resto de campos a través de
            // este puntero sin más casts
            saddr_ipv6 = (struct sockaddr_in6 *)saddr;
            // apuntamos al campo de la estructura que contiene la dirección
            addr = &(saddr_ipv6->sin6_addr);
            // obtenemos el puerto, pasando del formato de red al formato local
            port = ntohs(saddr_ipv6->sin6_port);
        }
        else if (saddr->ss_family == AF_INET)
        {   // IPv4
            printf("IPv4\n");
            saddr_ipv4 = (struct sockaddr_in *)saddr;            //----------------------1
            addr = &(saddr_ipv4->sin_addr);                      //----------------------2
            port = ntohs(saddr_ipv4->sin_port);                  //----------------------3
        }
        else
        {
            fprintf(stderr, "familia desconocida\n");
            exit(1);
        }
        // convierte la dirección ip a string
        inet_ntop(saddr->ss_family, addr, ipstr, sizeof ipstr);
        printf("\tDirección (interpretada según familia): %s\n", ipstr);
        printf("\tPuerto (formato local): %d\n", port);
    }
}
/**************************************************************************/
/* Configura el socket, devuelve el socket y servinfo */
/**************************************************************************/
int initsocket(struct addrinfo *servinfo, char f_verbose){
    int sock = -1;
    int numdir = 1;

    while (servinfo != NULL && sock < 0)
    {   // bucle que recorre la lista de direcciones
        printf("Intentando conexión con dirección %d:\n", numdir);

        // crea un extremo de la comunicación y devuelve un descriptor
        if (f_verbose)
        {
            printf("Creando el socket (socket)... ");
            fflush(stdout);
        }
        sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);                                                               //7, 8 ,9
        if (sock < 0)
        {
            perror("Error en la llamada socket: No se pudo crear el socket");
        } //else socket creado correctamente

        // "avanzamos" a la siguiente estructura de direccion
        servinfo = servinfo->ai_next;
        numdir++;
    }

    if (sock < 0)
    {
        perror("No se ha podido establecer la comunicación");
        exit(1);
    }

    return sock;
}


/**************************************************************************/
/* Comprueba si el mensaje es valido */
/**************************************************************************/
int esMensajeValido(struct rcftp_msg *mensaje_servidor, uint8_t version){
	if(issumvalid(mensaje_servidor, sizeof(*mensaje_servidor))== TRUE){
		if(mensaje_servidor->version == version){
            return TRUE;
        }else{
            fprintf(stderr,"Error, la versión es incorrecta. \n");
            return FALSE;
        }
    }else {
        fprintf(stderr,"Error, el checksum es incorrecto. \n");
        return FALSE;
    }
}

/**************************************************************************/
/* Comprueba si hemos obtenido la respuesta esperada */
/**************************************************************************/
int esLaRespuestaEsperada(struct rcftp_msg *mensaje_servidor, struct rcftp_msg *mensaje_cliente){
	if(mensaje_servidor->flags == F_ABORT){
		fprintf(stderr,"Error, el servidor a finalizado forzosamente la comunicación con el cliente. \n");
		return FALSE;
        //exit(1);
	}else if(mensaje_servidor->flags == F_BUSY){
		fprintf(stderr,"Error, el servidor está ocupado en este momento. \n");
		return FALSE;
        //sleep(5);
	}else if(mensaje_servidor->flags != F_FIN && mensaje_cliente->flags == F_FIN){
		fprintf(stderr,"Error, el servidor no ha mandado la flag F_FIN después de que el cliente sí la haya mandado. \n");
		return FALSE;
	}else if(htonl(mensaje_cliente->numseq) + htons(mensaje_cliente->len) != htonl(mensaje_servidor->next)){
        fprintf(stderr,"Error, el next que se ha recibido por parte del servidor, no coincide con la suma de numseq + len que mandó el cliente. \n");
        return FALSE;
    }else return TRUE;
}

/**************************************************************************/
/* Construye el struct rcftp_msg según los parámetros dados */
/**************************************************************************/
void construirMensajeRCFTP(struct rcftp_msg *mensaje_cliente, uint8_t version, int ultimoMensaje, ssize_t bytes_entrada_estandar, uint32_t numseq, int next) {
    mensaje_cliente->version = version;
    if(ultimoMensaje == FALSE){
		mensaje_cliente->flags = F_NOFLAGS;
	}else mensaje_cliente->flags = F_FIN;
    mensaje_cliente->numseq = numseq;
    mensaje_cliente->next = htonl(next);
    mensaje_cliente->len =  htons(bytes_entrada_estandar);
	mensaje_cliente->sum = 0;
    mensaje_cliente->sum = xsum((char *)mensaje_cliente, sizeof(*mensaje_cliente));
}




/**************************************************************************/
/*  algoritmo 1 (basico)  */
/**************************************************************************/
void alg_basico(int socket, struct addrinfo *servinfo) {

	//Declaración de variables principales.
	struct rcftp_msg mensaje_cliente;
	struct rcftp_msg mensaje_servidor;
	int ultimoMensaje = FALSE, ultimoMensajeConfirmado = FALSE;
	ssize_t bytes_entrada_estandar;

	printf("Comunicación con algoritmo básico\n");

	mensaje_servidor.next = htonl(0); //Empezamos next con 0.

	//Bucle principal, aquí se desarrolla toda la captación de datos por entrada estándar, estos se mandarán al servidor y recibiremos una respuesta del mismo.
	while(ultimoMensajeConfirmado == FALSE){

		//Leemos el mensaje escrito por entrada estándar.
		bytes_entrada_estandar = readtobuffer((char *)mensaje_cliente.buffer, RCFTP_BUFLEN);
		if(bytes_entrada_estandar <= 0) ultimoMensaje = TRUE;
		
		//Construimos el mensaje que enviaremos al servidor.
		construirMensajeRCFTP(&mensaje_cliente, RCFTP_VERSION_1, ultimoMensaje, bytes_entrada_estandar, mensaje_servidor.next, 0);
		//print_rcftp_msg(&mensaje_cliente , sizeof(mensaje_cliente));

        //Primero enviamos y recibimos. Si hay algún error en el mensaje recibido, volvemos a enviar y recibir. El servidor también nos puede dar indicaciones al programa mediante flags.
        do{
            //Enviamos el mensaje al servidor.
            if(sendto(socket, &mensaje_cliente, sizeof(mensaje_cliente), 0, servinfo->ai_addr, servinfo->ai_addrlen) < 0){
                perror("Error de escritura en el socket");
                exit(1);
            }

            //Recibimos una respuesta del servidor.
            ssize_t bytes_recibidos = recvfrom(socket, &mensaje_servidor, sizeof(mensaje_servidor), 0, servinfo->ai_addr, &servinfo->ai_addrlen);
            if(bytes_recibidos != sizeof(mensaje_servidor)){
                fprintf(stderr,"Recibidos %zd bytes en lugar de los %lu esperados\n", bytes_recibidos, sizeof(mensaje_servidor));
                exit(1);
            }
        }while(esMensajeValido(&mensaje_servidor, RCFTP_VERSION_1) == FALSE || esLaRespuestaEsperada(&mensaje_servidor, &mensaje_cliente) == FALSE);
		
        if(ultimoMensaje == TRUE) ultimoMensajeConfirmado = TRUE;
	}
}

/**************************************************************************/
/*  algoritmo 2 (stop & wait)  */
/**************************************************************************/
void alg_stopwait(int socket, struct addrinfo *servinfo) {

    //Declaración de variables principales.
	struct rcftp_msg mensaje_cliente;
	struct rcftp_msg mensaje_servidor;
	int ultimoMensaje = FALSE, ultimoMensajeConfirmado = FALSE, esperar, timeouts_procesados;
	ssize_t bytes_entrada_estandar;
    
    //Ponemos el socket en modo "no bloqueante".
    int socket_flags = fcntl(socket, F_GETFL, 0);
    fcntl(socket, F_SETFL, socket_flags | O_NONBLOCK);
	
    //Asociamos la rutina de servicio handle_sigalarm a la alarma antes de usar los temporizadores de multialarm. 
    signal(SIGALRM, handle_sigalrm);

    printf("Comunicación con algoritmo stop&wait\n");

	mensaje_servidor.next = htonl(0); //Empezamos next con 0.

	//Bucle principal, aquí se desarrolla toda la captación de datos por entrada estándar, estos se mandarán al servidor y recibiremos una respuesta del mismo.
	while(ultimoMensajeConfirmado == FALSE){

		//Leemos el mensaje escrito por entrada estándar.
		bytes_entrada_estandar = readtobuffer((char *)mensaje_cliente.buffer, RCFTP_BUFLEN);
		if(bytes_entrada_estandar <= 0) ultimoMensaje = TRUE;
		
		//Construimos el mensaje que enviaremos al servidor.
		construirMensajeRCFTP(&mensaje_cliente, RCFTP_VERSION_1, ultimoMensaje, bytes_entrada_estandar, mensaje_servidor.next, 0);
		print_rcftp_msg(&mensaje_cliente , sizeof(mensaje_cliente));

        //Primero enviamos y recibimos. Si hay algún error en el mensaje recibido, volvemos a enviar y recibir. El servidor también nos puede dar indicaciones al programa mediante flags.
        do{
            //Enviamos el mensaje al servidor.
            if(sendto(socket, &mensaje_cliente, sizeof(mensaje_cliente), 0, servinfo->ai_addr, servinfo->ai_addrlen) < 0){
                perror("Error de escritura en el socket");
                exit(1);
            }

            //Hacemos uso del temporizador que caracteriza a Stop&Wait y recibimos una respuesta del servidor.
            if(addtimeout() == TRUE){
                esperar = TRUE;
                timeouts_procesados = 0;
                while(esperar == TRUE){
                    ssize_t bytes_recibidos = recvfrom(socket, &mensaje_servidor, sizeof(mensaje_servidor), 0, servinfo->ai_addr, &servinfo->ai_addrlen);
                    if(bytes_recibidos > 0){
                        canceltimeout();
                        esperar = FALSE;
                    }else if(bytes_recibidos < 0 && errno != EAGAIN){
                        perror("Error en recvfrom");
                        exit(1);
                    }
                    
                    if(timeouts_procesados != timeouts_vencidos){
                        esperar = FALSE;
                        timeouts_procesados++;
                    }

                }
            }else{ 
                fprintf(stderr, "Error, no se ha podido añadir la alarma. \n");
                exit(1);
            }

        }while(esMensajeValido(&mensaje_servidor, RCFTP_VERSION_1) == FALSE || esLaRespuestaEsperada(&mensaje_servidor, &mensaje_cliente) == FALSE);
		
        if(ultimoMensaje == TRUE) ultimoMensajeConfirmado = TRUE;
	}
}

/**************************************************************************/
/*  algoritmo 3 (ventana deslizante)  */
/**************************************************************************/

/* Construcción de mensaje con parámetros específicos */
void construirMensajeRCFTP2(struct rcftp_msg *mensaje_cliente, 
                            const char *datos, 
                            const long int numseq, 
                            const int bytes_entrada, 
                            int ultimoMensaje) 
{
    mensaje_cliente->version = RCFTP_VERSION_1;
    mensaje_cliente->flags   = (ultimoMensaje) ? F_FIN : F_NOFLAGS;

    // Almacenamos numseq en orden de red.
    mensaje_cliente->numseq = htonl(numseq);
    mensaje_cliente->next   = htonl(0);

    // Convertimos la longitud a formato de red.
    mensaje_cliente->len    = htons(bytes_entrada);

    // Copiamos 'bytes_entrada' del buffer 'datos' al buffer del mensaje.
    for (int i = 0; i < bytes_entrada; i++) {
        mensaje_cliente->buffer[i] = datos[i];
    }

    // Ajustamos checksum (inicializamos a 0 y calculamos).
    mensaje_cliente->sum = 0;
    mensaje_cliente->sum = xsum((char *)mensaje_cliente, sizeof(*mensaje_cliente));
}

/* Comprueba si la respuesta es la esperada para GBN. */
int esRespuestaEsperadaGBN(struct rcftp_msg mensaje_servidor,
                           struct rcftp_msg mensaje_cliente,
                           const int finLocal,
                           const int ultimaSecuenciaConfirmada,
                           const int posicionEnvio)
{
    // 1) Comprobamos versión y checksum
    if (mensaje_servidor.version != RCFTP_VERSION_1 ||
        issumvalid(&mensaje_servidor, sizeof(mensaje_servidor)) == 0)
    {
        return 0;
    }

    // 2) Flags conflictivos (ABORT o BUSY)
    if (mensaje_servidor.flags == F_ABORT || mensaje_servidor.flags == F_BUSY) {
        return 0;
    }

    // 3) Si ya estamos en fin local, aceptamos cualquier respuesta para cerrar
    if (finLocal) {
        return 1; 
    }

    // 4) Chequeamos 'next' dentro del rango esperado
    uint32_t nextDelServidor = ntohl(mensaje_servidor.next);
    // Explicación: se espera que nextDelServidor sea > ultimaSecuenciaConfirmada
    // y <= posicionEnvio.
    if (nextDelServidor <= (uint32_t)ultimaSecuenciaConfirmada ||
        nextDelServidor >  (uint32_t)posicionEnvio)
    {
        return 0;
    }

    // 5) En caso contrario, es una respuesta válida.
    return 1;
}

/* Envía un mensaje RCFTP al servidor. */
void enviarMensaje(struct rcftp_msg *mensaje_cliente, int socket, struct addrinfo *servinfo)
{
    char f_verbose = 1;          // Verbosidad local
    int  tam       = sizeof(struct rcftp_msg);

    int numBytes = sendto(socket,
                          (char*)mensaje_cliente,
                          tam,
                          0,
                          servinfo->ai_addr,
                          servinfo->ai_addrlen);

    if (numBytes < 0) {
        perror("Error de escritura en socket");
        exit(1);
    } 
    else {
        if (f_verbose) {
            printf("  Enviados %i bytes (de %d esperados) al servidor\n", numBytes, tam);
        }
    }
}

/* Recibe un mensaje RCFTP (modo no bloqueante). 
   Devuelve el número de bytes leídos o -1 si no hay nada. */
int recibirMensaje(struct rcftp_msg *mensaje_servidor, int sock, struct addrinfo *servinfo)
{
    int rxBytes = recvfrom(sock,
                           mensaje_servidor,
                           sizeof(struct rcftp_msg),
                           0,
                           servinfo->ai_addr,
                           &servinfo->ai_addrlen);

    if (rxBytes < 0) {
        // Controlamos si no hay datos disponibles.
        if (errno != EAGAIN) {
            perror("Error en recvfrom");
            exit(1);
        } else {
            // No había nada que leer en este momento.
            printf("  [No hay datos disponibles en este momento]\n");
            return -1;
        }
    }

    // Verificamos que el tamaño recibido coincida exactamente con el de rcftp_msg.
    if (rxBytes != (int)sizeof(*mensaje_servidor)) {
        printf("Error: se recibieron %d bytes en lugar de %lu\n",
               rxBytes,
               (unsigned long)sizeof(*mensaje_servidor));
        exit(1);
    }

    printf("Recibidos %ld bytes\n", (long)sizeof(*mensaje_servidor));
    return rxBytes;
}

/**
 * Implementación del algoritmo GBN (go-back-n) con ventana deslizante.
 *    - Introducimos una variable 'reintentos' para contar reenvíos .
 *    - Añadimos mensajes extra en los puntos clave.
 */
void alg_ventana(int socket, struct addrinfo *servinfo, int window)
{
    printf("Comunicación con algoritmo go-back-n\n");

    /* 1) Configuramos el socket en modo no bloqueante */
    int socket_flags = fcntl(socket, F_GETFL, 0);
    fcntl(socket, F_SETFL, socket_flags | O_NONBLOCK);

    /* 2) Asignamos la rutina de manejo de señal para los timeouts */
    signal(SIGALRM, handle_sigalrm);

    /* 3) Declaramos las estructuras principales para rcftp_msg y buffers */
    struct rcftp_msg mensaje_cliente;
    struct rcftp_msg mensaje_servidor;
    struct rcftp_msg mensaje_retry;  // se llamaba 'mensaje_reenvio'
    char datos[RCFTP_BUFLEN];

    /* 4) Variables de control */
    int ultimoMensajeConfirmado = FALSE;  
    int timeoutsProcesados      = 0;
    int reintentos              = 0;  // contador de reintentos tras timeout

    int numBytes;               // bytes a enviar
    int numBytesRecibidos;      // bytes recibidos del servidor
    int secuenciaActual  = 0;   // secuencia local (fichero/enviado)
    int ultimaSecuencia  = 0;   // última secuencia confirmada por el servidor
    int longitudAntigua  = RCFTP_BUFLEN;  // para reenvío
    int secuenciaAntigua;                // para reenvío

    /* 5) Ajustamos el tamaño de la ventana de emisión */
    setwindowsize(window);
    printf("Ventana configurada con tamaño: %d\n", window);

    /* Bucle principal: enviamos datos mientras no hayamos terminado. */
    while (!ultimoMensajeConfirmado)
    {
        /**************************************************************
         * BLOQUE DE ENVÍO
         **************************************************************/
        if (getfreespace() >= RCFTP_BUFLEN)
        {
            /* Leemos datos del "fichero" (entrada estándar) */
            numBytes = readtobuffer(datos, RCFTP_BUFLEN);

            if (numBytes < 0) {
                perror("Error en readtobuffer");
                exit(1);
            }
            else if (numBytes == 0) {
                /* EOF => construimos un paquete con 0 bytes y F_FIN */
                construirMensajeRCFTP2(&mensaje_cliente,
                                       datos,
                                       secuenciaActual,
                                       0,
                                       TRUE);
                printf("Enviando paquete FIN (EOF), secuenciaActual = %d\n", secuenciaActual);
                enviarMensaje(&mensaje_cliente, socket, servinfo);
                addtimeout();
            }
            else {
                /* Hemos leído 'numBytes', comprobamos si es el último fragmento */
                int esUltimo = (numBytes < RCFTP_BUFLEN) ? TRUE : FALSE;

                construirMensajeRCFTP2(&mensaje_cliente,
                                       datos,
                                       secuenciaActual,
                                       numBytes,
                                       esUltimo);
                printf("Enviando paquete con secuencia = %d, longitud = %d\n",
                       secuenciaActual, numBytes);

                enviarMensaje(&mensaje_cliente, socket, servinfo);
                addtimeout();

                /* Añadimos los datos a la ventana de emisión */
                addsentdatatowindow(datos, numBytes);

                /* Actualizamos la secuencia local */
                secuenciaActual += numBytes;
            }
        }

        /**************************************************************
         * BLOQUE DE RECEPCIÓN
         **************************************************************/
        numBytesRecibidos = recibirMensaje(&mensaje_servidor, socket, servinfo);

        if (numBytesRecibidos > 0) {
            /* Comprobamos si la respuesta del servidor era la esperada */
            if (esRespuestaEsperadaGBN(mensaje_servidor, 
                                       mensaje_cliente,
                                       /* finLocal        */ ultimoMensajeConfirmado,
                                       /* ultimaSecuencia */ ultimaSecuencia,
                                       /* posicionEnvio   */ secuenciaActual))
            {
                /* Cancelamos el timeout, liberamos ventana hasta 'next' */
                canceltimeout();
                freewindow(ntohl(mensaje_servidor.next));

                /* Actualizamos la última secuencia confirmada */
                ultimaSecuencia = ntohl(mensaje_servidor.next);

                /* Si el servidor manda F_FIN, es que hemos terminado */
                if (mensaje_servidor.flags == F_FIN) {
                    ultimoMensajeConfirmado = TRUE;
                }
            }
        }

        /**************************************************************
         * BLOQUE DE TIMEOUT
         **************************************************************/
        if (timeoutsProcesados != timeouts_vencidos)
        {
            /* Si detectamos que se ha incrementado timeouts_vencidos => reenvío */
            reintentos++;
            printf("Timeout detectado. Reintento #%d\n", reintentos);

            /* Obtenemos datos a reenviar de la ventana */
            longitudAntigua  = RCFTP_BUFLEN;
            secuenciaAntigua = getdatatoresend(datos, &longitudAntigua);

            /* Reconstruimos y reenviamos el mensaje */
            construirMensajeRCFTP2(&mensaje_retry, 
                                   datos,
                                   secuenciaAntigua, 
                                   longitudAntigua,
                                   (longitudAntigua != RCFTP_BUFLEN));
            printf("Reenviando paquete con secuenciaAntigua = %d, longitud = %d\n",
                   secuenciaAntigua, longitudAntigua);

            enviarMensaje(&mensaje_retry, socket, servinfo);
            addtimeout();

            timeoutsProcesados++;
        }
    }

    /* Cuando salimos del bucle, significa que hemos completado la transmisión. */
    printf("Fin de la comunicación con algoritmo go-back-n\n");
}