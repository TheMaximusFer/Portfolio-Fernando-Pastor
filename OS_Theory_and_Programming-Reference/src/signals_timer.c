#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
int CronometroON = FALSE;

void capturaCtrlC(int sig) {
    static struct timeval tiempo_inicial, tiempo_final;
    static long seconds, useconds;
    if(CronometroON == FALSE){
        gettimeofday(&tiempo_inicial, NULL);
        fprintf(stdout, "-- Inicio de cronómetro, señal de captura %d --\n", sig);
        CronometroON = TRUE;
    }else{
        gettimeofday(&tiempo_final, NULL);
        fprintf(stdout, "-- Fin de cronómetro, señal de captura %d --\n", sig);
        seconds = tiempo_final.tv_sec - tiempo_inicial.tv_sec;
        useconds = tiempo_final.tv_usec - tiempo_inicial.tv_usec;
        if (useconds < 0) {
            seconds -= 1;
            useconds += 1000000;
        }
        fprintf(stdout, "**Tiempo transcurrido: %ld segundos y %ld microsegundos**\n", seconds, useconds);
        CronometroON = FALSE;
    }
}

void capturaCtrlY(int sig) {
    fprintf(stdout, "\nSaliendo del programa voluntariamente, señal de captura %d\n", sig);
    exit(0);
}


void capturaAlarm(int sig) {
    fprintf(stdout, "\nSaliendo del programa por shareware (60s), señal de captura %d\n", sig);
    exit(0);
}

int main() {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = capturaCtrlC;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);
    sa.sa_handler = capturaCtrlY;
    sigaction(SIGQUIT, &sa, NULL);
    sa.sa_handler = capturaAlarm;
    sigaction(SIGALRM , &sa, NULL);

    alarm(60);

    while (TRUE){
        if(CronometroON == FALSE) printf("\nEsperando señales...\n");
        pause();
    }
}