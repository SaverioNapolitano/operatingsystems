#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork e exec
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <ctype.h>

int main(int argc, char** argv) {
    
    if (argc < 3) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    char c;
    int tot = 0;
    int oldtot = -1;
    int N = argc - 1;
    int p[2][2];
    if(pipe(p[0])<0){
        printf("Errore in pipe\n");
        exit(5);
    }
    if(pipe(p[1])<0){
        printf("Errore in pipe\n");
        exit(6);
    }
    for(int i = 0; i < N; i++){
        int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(2);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            //controllo se il file e' accedibile
            int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(-1);
            }

            if((i+1)%2==0){
                /*file pari, leggo solo caratteri numerici, uso p[1]*/
                close(p[0][0]);
                close(p[0][1]);
                close(p[1][0]);
                while(read(fd, &c, 1) != 0){ /*fino a che ci sono caratteri da leggere nel file*/
                    if(isdigit(c)){ /*se numero lo scrivo sulla pipe*/
                        write(p[1][1], &c, 1);
                    }
                }
            } else {
                /*file dispari, leggo solo caratteri alfabetici, uso p[0]*/
                close(p[0][0]);
                close(p[1][1]);
                close(p[1][0]);
                while(read(fd, &c, 1) != 0){ /*fino a che ci sono caratteri da leggere nel file*/
                    if(isalpha(c)){ /*se carattere lo scrivo sulla pipe*/
                        write(p[0][1], &c, 1);
                    }
                }
            }
            exit(c);
        }
        /* processo padre */
    }
    close(p[0][1]);
    close(p[1][1]);
    while(oldtot != tot){
        oldtot = tot;
        if(read(p[0][0], &c, 1) != 0){
            printf("%c ", c);
            tot++;
        }
        if(read(p[1][0], &c, 1) != 0){
            printf("%c ", c);
            tot++;
        }
    }
    printf("\nCaratteri ricevuti: %d\n", tot);
    
    for(int i = 0; i < N; i++){
        int pidFiglio;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(4);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Il figlio con pid %d ha ritornato %d che corrisponde al carattere %c\n", pidFiglio, ritorno, (char)ritorno);
        }
        
    }
    
    exit(0);
}