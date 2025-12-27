
/*
No estoy seguro de si el programa funciona.

---ESTRUCTURA MÁS TÍPICA PARA PROGRAMAR TUBERÍAS---
pipe()
if fork() == 0 --> stdout = fd[1]; execvp();
if fork() == 0 --> stdin = fd[0]; execvp();
close(fd[0]);
close(fd[1]); 
wait();
wait();
exit(0);

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]){
    char *lineas[5];
    char *argumentos[100];
    close(0);
    open(argv[1], O_RDONLY);
    
    for(i = 0; i < 5; i++) {
        lineas[i] = gets();
    }

    switch (fork()){
        case -1:
            perror("fork");
            return 1;
        case 0:
            int fd[2], i;
            pipe(fd);
            switch (fork()){
                case -1:
                    perror("fork");
                    return 1;
                case 0: //HIJO
                    close(1);
                    open(lineas[3], O_RDONLY);

                    close(2);
                    open(lineas[4], O_RDONLY);

                    close(fd[1]);
                    close(0);
                    dup(fd[0]);
                    close(fd[0]);
                    
                    char *token = strtok(lineas[1]," ");
                    for(i = 0; token != NULL; i++){
                        argumentos[i] = token;
                        token = strtok(NULL, " ");
                    }
                    argumentos[i] = NULL;

                    execvp(argumentos[0], &argumentos[0]);

                default://PADRE
                    close(0);   
                    open(lineas[2], O_RDONLY);

                    close(2);
                    open(lineas[4], ORDONLY);
                    
                    close(fd[0]);
                    close(1);
                    dup(fd[1]);
                    close(fd[1]);

                    char *token = strtok(lineas[0]," ");
                    for(i = 0; token != NULL; i++){
                        argumentos[i] = token;
                        token = strtok(NULL, " ");
                    }
                    argumentos[i] = NULL;

                    execvp(argumentos[0], &argumentos[0]);

            }
        default:
            // Parent process
            wait(NULL);
            break;
    }
}