#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

int main(int argc, char **argv){
    int N; /*numero di file*/
    int i; /*indice per scorrere i file*/
    char Cx; /*contiene il carattere passato come ultimo parametro*/
    int pid; /*pid per fork*/
    char C; /*contiene il carattere letto dal file*/
    int pidFiglio, ritorno, status; /*per la wait*/
    /*controllo che siano passati almeno 3 parametri*/
    if(argc < 4){
        printf("Errore: necessari almeno 3 parametri ma argc = %d\n", argc);
        exit(1);
    }
    N = argc - 2;
    /*controllo che ultimo parametro sia un carattere*/
    if(strlen(argv[argc-1]) != 1){
        printf("Errore: %s non singolo carattere\n", argv[argc-1]);
        exit(3);
    }

    /*isolo il carattere per maggiore comodita'*/
    Cx = argv[argc-1][0];
    /*creazione figli*/
    for(i = 0; i < N; i++){
        if((pid = fork()) < 0){
            /*fork fallita*/
            printf("Errore nella fork\n");
            exit(4);
        }
        if(pid == 0){
            /*figlio*/
            int occorrenze = 0;
            int fd = open(argv[i+1], O_RDONLY);
            if(fd < 0){
                /*open fallita*/
                exit(255);
            }
            /*fino a quando ci sono caratteri da leggere*/
            while(read(fd, &C, 1) > 0){
                if(C == Cx){
                    /*se il carattere corrisponde a quello cercato*/
                    occorrenze++;
                }
            }
            exit(occorrenze);
        }
    }

    for(i = 0; i < N; i++){
        pidFiglio = wait(&status);
        if(pidFiglio < 0){
            printf("Errore: non c'erano figli da attendere\n");
            exit(4);
        }
        if((status & 0xFF) != 0){
            printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        }
        else{
            ritorno = (int)((status >> 8) & 0xFF);
            printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi)\n", pidFiglio, ritorno);
        }

    }
    exit(0);

}