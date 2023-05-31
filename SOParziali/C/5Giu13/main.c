#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <errno.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    
    char s[256]; /*array di caratteri che conterrà la linea letta dal file*/
    int N; /*numero di processi figli*/
    int i; /*indice*/
    int k; /*indice*/
    int pid;	// memorizzo il valore di ritorno della funzione fork
    int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    pipe_t* piped; /*pipe per comunicazione padre-figli*/

    N = argc - 1;

    //apro N pipes
    piped = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(1);
    }
    
    for(int i = 0;i < N;i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(2);
        }
    }

    for(i = 0; i < N; i++){
        
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(1);
        }
        
        if (pid == 0)
        {	/* processo figlio */
            //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
            for(k = 0; k < N; k++){
                close(piped[k][0]);
                if (k != i) {
                    close(piped[k][1]);
                }
            }
            //controllo se il file e' accedibile
            if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(-1);
            }
            
            //chiudo lo stdout
            close(1);
            //duplico la pipe lato scrittura
            dup(piped[i][1]);
            //chiudo la pipe dato che non mi serve più
            close(piped[i][1]);

            execlp("head", "head", "-1", argv[i+1], (char *)0);
            /* si esegue l'istruzione seguente SOLO in caso di fallimento */
            
            perror("errore esecuzione comando"); // 
            exit(errno);
        }
        
    }
    /* processo padre */
    //chiudo tutte le pipe in scrittura
    for(k = 0; k < N; k++){
        close(piped[k][1]);
    }

    for(i=0; i<N; i++){
        k = 0;
        while (read(piped[i][0], &(s[k]), sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
        {
            if(s[k]=='\n'){ /*se ho trovato il terminatore di linea*/
                s[k]=0; /*lo converto in terminatore di stringa*/
                break; /*ed esco dal ciclo*/
            }
            ++k;
        }
        printf("Il figlio di indice %d associato al file %s ha letto la linea '%s'\n", i, argv[i+1], s);
    }

    /*padre aspetta i figli*/
    for(i=0; i<N; i++){
        if ((pid = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(4);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Il figlio con pid %d ha ritornato %d\n", pid, ritorno);
        }
    }


    exit(0);
}