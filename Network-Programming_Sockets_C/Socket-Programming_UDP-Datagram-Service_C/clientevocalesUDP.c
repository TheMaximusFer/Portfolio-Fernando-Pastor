/**
 * @file clientevocalesTCP.c
 *
 * Programa *clientevocalesTCP* que envía cadenas de texto a un servidor.
 *
 * Uso: clientevocalesTCP servidor puerto
 *
 * El programa crea un socket TCP y lo conecta al servidor y puerto especificado.
 * A través del socket envía cadenas de caracteres hasta llegar a fin de fichero
 * (Control+d para provocarlo desde la entrada estándar).
 * Finalmente, espera como respuesta el número total de vocales en las
 * cadenas enviadas e imprime dicho valor por pantalla.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include "comun.h"

// definición de constantes
#define MAX_BUFF_SIZE 1000 ///< Tamaño del buffer para las cadenas de texto.

/**
 * Función que crea la conexión y se conecta al servidor.
 *
 * @param servinfo Estructura de dirección a la que conectarse.
 * @param f_verbose Flag.
 * @return Descriptor de socket.
 */
int initsocket(struct addrinfo *servinfo, char f_verbose)
{
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

/**
 * Programa principal
 *
 * @param argc Número de argumentos usados en la línea de comandos.
 * @param argv Vector de punteros a cadenas de caracteres. argv[0]
 *             apunta al nombre del programa, argv[1] al primer
 *             argumento y así sucesivamente.
 * @return 0 si todo ha ido bien, distinto de 0 si hay error.
 */
int main(int argc, char * argv[])
{
    // declaración de variables propias del programa principal (locales a main)
    char f_verbose = 1;         // flag, 1: imprimir información extra
    struct addrinfo * servinfo; // puntero a estructura de dirección destino
    int sock;                   // descriptor del socket
    char msg[MAX_BUFF_SIZE];    // buffer donde almacenar datos para enviar
    ssize_t len,       // número de bytes leídos por la entrada estándar
            sentbytes, recvbytes;
                       // número de bytes enviados/recibidos por el socket
    uint32_t num;      // variable donde anotar el número de vocales
    const char fin[1] = {4}; // carácter ASCII end of transmission (EOT)

    // verificación del número de parámetros:
    if (argc != 3)
    {
        printf("Número de parámetros incorrecto \n");
        printf("Uso: %s servidor puerto/servicio\n", argv[0]);
        exit(1); // finaliza el programa indicando salida incorrecta (1)
    }

    // obtiene estructura de direccion
    servinfo = obtener_struct_direccion(argv[1], argv[2], f_verbose);

    // crea un extremo de la comunicación con la primera de las
    // direcciones de servinfo e inicia la conexión con el servidor.
    // Devuelve el descriptor del socket
    sock = initsocket(servinfo, f_verbose);


    // bucle que lee texto del teclado y lo envía al servidor
    printf("\nTeclea el texto a enviar y pulsa <Enter>, o termina con <Ctrl+d>\n");
    while ((len = read(0, msg, MAX_BUFF_SIZE)) > 0)
    {
        if (f_verbose) printf("  Leídos %zd bytes\n", len);

        // envía datos al socket
        if ((sentbytes = sendto(sock, msg, len, 0, servinfo->ai_addr, servinfo->ai_addrlen)) < 0)                                                                                                  //13, 14, 15
        {
            perror("Error de escritura en el socket");
            exit(1);
        }
        else
        {
            if (f_verbose) printf("  Enviados %zd bytes al servidor\n", sentbytes);
        }

        printf("Teclea el texto a enviar y pulsa <Enter>, o termina con <Ctrl+d>\n");
    }

    //terminamos de enviar el texto
    sendto(sock, fin, 1, 0, servinfo->ai_addr, servinfo->ai_addrlen);

    // ya no queremos enviar más, cerramos para escritura
    if (f_verbose)
    {
        printf("Cerrando el socket para escritura...");
        fflush(stdout);
    }

    if (f_verbose)
    {
        printf("hecho\nEsperando respuesta del servidor...");
        fflush(stdout);
    }

    // recibe del servidor el número de vocales recibidas:
    if ((recvbytes = recvfrom(sock, &num, sizeof(num), 0, servinfo->ai_addr, &servinfo->ai_addrlen)) != sizeof num)                                                                                   //18, 19, 20
    {
        printf("Recibidos %zd bytes en lugar de los %lu esperados\n", recvbytes, sizeof num);
        exit(1);
    }

    // hay que liberar la memoria dinámica usada para la dirección
    freeaddrinfo(servinfo);
    servinfo = NULL;

    printf(" %lu bytes\n", sizeof num);
    printf("Todo el texto enviado contenía en total %d vocales\n", ntohl(num));                                                                         //21

    // cierra la conexión del socket
    if (close(sock) < 0)                                                                                                                                //22
    {
        perror("Error al cerrar el socket");
        exit(1);
    }
    else
    {
        if (f_verbose) printf("Socket cerrado\n");
    }

    exit(0); // finaliza el programa indicando salida correcta (0)
}
