#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <errno.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];
int main(int argc, char** argv) {
    char c; /*carattere letto dalla pipe dal figlio*/
    int M; /*numero di figli che verranno generati dal padre con la fork*/
    int i; /*indice*/
    int k; /*indice*/
    int lunghezza; /*lunghezza dell'ultima linea del file*/
    int pid;	// memorizzo il valore di ritorno della funzione fork
    int fd; /*variabile che conterra il file descriptor del file che verrà aperto con la open */
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    pipe_t* piped; /*pipe per comunicazione figli-padre*/

    if (argc < 3) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    M = argc - 1;

    //allocazione della memoria per array dinamico di M pipe
    piped = (pipe_t*)malloc(sizeof(pipe_t) * M);
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(2);
    }
    
    /*creazione delle M pipe*/
    for(i = 0;i < M;i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(3);
        }
    }

    /*Ciclo di generazione dei figli*/
    for(i = 0; i < M; i++){
        
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(4);
        }
        
        if (pid == 0)
        {	/* processo figlio */

            //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
            for(k = 0; k < M; k++){
                close(piped[k][0]);
                if (k != i) {
                    close(piped[k][1]);
                }
            }
            pipe_t pipedNF; /*pipe per comunicazione nipote-figlio*/
            
            if (pipe(pipedNF) < 0 )
            {	printf("Errore creazione pipe\n");
                exit(-1);
            }

            if ((pid = fork()) < 0)	/* Il processo figlio crea un nipote */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(-1);
            }
            
            if (pid == 0)
            {	/* processo nipote */
                close(piped[i][1]); /*chiudo l'unica pipe padre-figlio lasciata aperta dal figlio*/
                close(1); /*chiudo lo standard input per poterlo ridirezionare sulla pipe*/
                dup(pipedNF[1]);
                /*chiudo la pipe siccome non serve più*/
                close(pipedNF[0]);
                close(pipedNF[1]);

                //controllo se il file e' accedibile   
                if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                    printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                    exit(-1);
                }

                execlp("tail", "tail", "-1", argv[i+1], (char *)0); // in caso di lista, ricordati di 0 terminare la lista dei parametri!
                /* si esegue l'istruzione seguente SOLO in caso di fallimento */
                
                perror("errore esecuzione comando");
                exit(errno);
            }
            /* processo figlio */
            close(pipedNF[1]); /*chiudo lato scrittura della pipe col nipote*/
            lunghezza = 0;
            while (read(pipedNF[0], &c, sizeof(char)))	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(c == '\n'){ /*se ho trovato il terminatore di linea*/
                    break; /*esco dal ciclo*/
                }
                ++lunghezza; /*altrimenti incremento il contatore e continuo a leggere*/
            }

            write(piped[i][1], &lunghezza, sizeof(lunghezza)); /*figlio comunica al file la lunghezza calcolata*/
            
            /*figlio aspetta nipote*/
            if ((pid = wait(&status)) < 0) {
                printf("Non e' stato creato nessun processo figlio\n");
                exit(-1);
            }
            ritorno = -1;
            if ((status & 0xFF) != 0)
            {
                printf("Il processo figlio è stato terminato in modo anomalo\n");
            } else {
                ritorno = (status >> 8) & 0xFF;
            }

            exit(ritorno);
        }
    }
    /* processo padre */
    //chiudo tutte le pipe in scrittura
    for(k = 0; k < M; k++){
        close(piped[k][1]);
    }
    for(i = M - 1; i >= 0; i--){ /*padre legge dalle pipe rispettando l'ordine inverso dei file*/
        read(piped[i][0], &lunghezza, sizeof(lunghezza)); /*padre legge dalla pipe corrispondente la lunghezza calcolata dal figlio*/
        printf("Il figlio di indice %d associato al file %s ha calcolato la lunghezza %d\n", i, argv[i+1], lunghezza);
    }

    /*padre aspetta i figli*/
    for(i = 0; i < M; i++){
        if ((pid = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(5);
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