#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork e exec
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
#include <errno.h>
#define PERM 0640
int main(int argc, char** argv) { 
    int i;
    int pidFiglio, pidNipote;	// memorizzo il valore di ritorno della funzione fork
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    for(i = 0; i < argc; i++){
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(2);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            int FOut;
            char name[256] = "";
            strcat(name, argv[i+1]);
            strcat(name, ".sort");
            close(1);
            FOut = open(name, O_WRONLY | O_CREAT | O_TRUNC, PERM);
            if(FOut < 0){
                perror("Errore nella creat\n"); // #include <errno.h>
                exit(errno);
            }
        
            if ((pidNipote = fork()) < 0)	/* Il processo padre crea un figlio */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(3);
            }
            
            if (pidNipote == 0)
            {	/* processo figlio */
                close(0);
                open(argv[i+1], PERM);

                execlp("sort", "sort", (char *) 0);

                perror("Errore nella exec\n");
                exit(-1);
            }
            if ((pidFiglio = wait(&status)) < 0) {
                printf("Non e' stato creato nessun processo figlio\n");
                exit(6);
            }
            
            if ((status & 0xFF) != 0)
            {
                printf("Il processo figlio è stato terminato in modo anomalo\n");
                exit(7);
            } else {
                ritorno = (status >> 8) & 0xFF;
            }
            exit(ritorno);
        }
    }
    
    for(i = 0; i < argc - 1; i++){
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
        printf("Il processo figlio ha PID %d e valore di ritorno %d\n", pidFiglio, ritorno);
    }
    exit(0);
}