#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define TRUE 1
#define FALSE 0

int modo = 1;

void captura_SIGUSR1(int signal) {
    modo = (modo % 3) + 1; // 1->2, 2->3, 3->1
    printf("Modo actual: %d\n", modo);  
}

void captura_CtrlC(int signal) {
    printf("\nRecibida señal SIGINT (Ctrl+C). Saliendo del programa...\n");
    _exit(0); // Termina el proceso
}

int main(int argc, char *argv[]){
    
    if(argc < 3) {
        fprintf(stderr, "Uso: %s fich_salida comando [paramaetros]\n", argv[0]);
        return 1;
    }
    
    printf("---------------------------------------------------------------\n"
           "Modos:\n"
           "  1: Ejecuta el comando y muestra su salida por pantalla.\n"
           "  2: Redirige la salida del comando al fichero especificado.\n"
           "  3: Alterna entre los modos 1 y 2.\n"
           "---------------------------------------------------------------\n"
           "En cualquier momento, puedes enviar la señal SIGUSR1 para cambiar de modo.\n"
           "Manda en otra terminal: kill -USR1 <PID>\n"
           "---------------------------------------------------------------\n"
           "PID: %d\n"
           "---------------------------------------------------------------\n\n", getpid() 
        );

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = captura_SIGUSR1;
    sigaction(SIGUSR1, &sa, NULL);
    sa.sa_handler = captura_CtrlC;
    sigaction(SIGINT, &sa, NULL);

    while(TRUE){ 
        sleep(4);  
        if(modo == 1 || modo == 3){
            switch(fork()){
                case -1:
                    perror("Error al crear el proceso hijo (modo 1)\n");
                    return 1;
                case 0: // Proceso hijo
                    execvp(argv[2], &argv[2]);
                    perror("Error en exec del proceso hijo (modo 1)\n");
                    return 1;
                default: // Proceso padre
                    if(wait(NULL) == -1){
                        perror("Error en wait (modo 1)\n");
                        return 1;
                    }
            }
            printf("\n");
        }

        if(modo == 2 || modo == 3){
            switch(fork()){
                case -1:
                    perror("Error al crear el proceso hijo (modo 2)\n");
                    return 1;
                case 0: // Proceso hijo
                    close(1);
                    open(argv[1], O_WRONLY);
                    execvp(argv[2], &argv[2]);
                    perror("Error en exec del proceso hijo (modo 2)\n");
                    return 1;
                default: // Proceso padre
                    if(wait(NULL) == -1){
                        perror("Error en wait (modo 2)\n");
                        return 1;
                    }
            }
        }

    }
}

