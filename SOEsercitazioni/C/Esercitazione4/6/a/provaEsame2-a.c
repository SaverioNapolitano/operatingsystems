#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork e exec
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
typedef int pipe_t[2]; /* definizione del TIPO pipe_t come array di 2 interi */

int main(int argc, char** argv) {
    
    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    int N = argc - 2;
    for(int i = 2; i < argc; i++){
        if (strlen(argv[i]) != 1) {
            printf("Errore, la stringa %s non e' un singolo carattere\n", argv[i]);
            exit(4);
        }
    }
    long int count;
    pipe_t *piped; /* array dinamico di pipe descriptors per comunicazioni figli-padre */
    piped = (pipe_t *) malloc (N*sizeof(pipe_t));
    if (piped == NULL){
        printf("Errore nella allocazione della memoria\n");
        exit(2); 
    }
    /* Creazione delle M pipe figli-padre */
    for(int j = 0; j < N; j++){
        if(pipe(piped[j])<0){
            printf("Errore nella creazione della pipe\n");
            exit(5);
        }
    }
    for(int i = 0; i < N; i++){
        int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(3);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
        /* Chiusura delle pipe non usate nella comunicazione con il padre */ 
        for (int k=0; k < N; k++){
            close(piped[k][0]);
            if (k != i) close(piped[k][1]); 
            }
            count = 0;
            char c;
            //controllo se il file e' accedibile
            int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
                exit(-1);
            }
            while(read(fd, &c, 1) != 0){
                if(c == argv[i+2][0]){
                    count++;
                }
            }
            write(piped[i][1], &count, sizeof(count));
            exit(argv[i+2][0]);
        }
        /* processo padre */
        close(piped[i][1]);
    }
    for(int i = 0; i < N; i++){
        read(piped[i][0], &count, sizeof(count));
        printf("Trovate %ld occorrenze del carattere %c\n", count, argv[i+2][0]);
    }
    for(int i = 0; i < N; i++){
        int pidFiglio;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(6);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Il figlio con pid %d ha ritornato %c\n", pidFiglio, (char)ritorno);
        }
        
    }
    exit(0);
}