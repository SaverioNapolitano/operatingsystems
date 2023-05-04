#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close e fork
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait


int main(int argc, char** argv) {
    int pid; /*per fork*/
    int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
    int pidFiglio;
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    if (argc != 2) /* controllo sul numero di parametri: devono essere in numero uguale a 1*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    
    if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
    {	/* In questo caso la fork e' fallita */
        printf("Errore durante la fork\n");
        exit(2);
    }
    
    if (pid == 0)
    {	/* processo figlio */
        close(0);
        //controllo se il file e' accedibile
        if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
            printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
            exit(3);
        }
        execl("mycat", "mycat", (char *) 0);
        exit(0);
    }
    /* processo padre */
    if ((pidFiglio = wait(&status)) < 0) {
        printf("Non e' stato creato nessun processo figlio\n");
        exit(4);
    }
    
    if ((status & 0xFF) != 0)
    {
        printf("Il processo figlio è stato terminato in modo anomalo\n");
        exit(5);
    } else {
        ritorno = (status >> 8) & 0xFF;
    }
    printf("Pid figlio=%d e valore ritornato=%d\n", pidFiglio, ritorno);

    exit(0);
}