#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <ctype.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];
int main(int argc, char** argv) {

    char car; /*carattere letto correntemente dai figli dal proprio file*/
    int trovati; /*numero di caratteri numerici trovati dal figlio nel file*/
    int nro; /*rappresentazione intera del carattere letto*/
    int N; /*numero di processi figli*/
    int n; /*indice dei processi figli*/
    int j; /*indice per cicli*/
    int pid;	// memorizza il valore di ritorno della funzione fork
    int fd; /*variabile che conterrà il file descriptor del file che verrà aperto con la open */
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    long int somma; /*valore che i figli devono comunicare al padre*/
    pipe_t* piped; /*pipe di comunicazione fra padre e figli*/

    
    if (argc < 2 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    N = argc - 1;

    //allocazione memoria per array dinamico di N pipes
    piped = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(2);
    }
    /*creazione delle N pipes*/
    for(n = 0; n < N; n++){
        if(pipe(piped[n])<0){
            printf("Errore nella creazione della pipe numero:%d\n",n);
            exit(3);
        }
    }

    for(n = 0; n < N; n++){
        
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(4);
        }
        
        if (pid == 0)
        {	/* processo figlio */
            //figlio non legge da nessuna pipe e scrive solo su quella di indice n
            for(j = 0; j < N; j++){
                close(piped[j][0]);
                if (j != n) {
                    close(piped[j][1]);
                }
            }
            //controllo se il file e' accedibile
            
            if((fd = open(argv[n+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[n+1], fd);
                exit(-1); /*si ritorna in caso di errore -1 che verrà interpretato come 255 che non è un valore accettabile*/
            }

            somma = 0L; /*inizializzo la somma*/
            trovati = 0; /*inizializzo il numero di caratteri trovati*/
            while (read(fd, &car, sizeof(char)))	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(isdigit(car)){ /*se il carattere letto è numerico*/
                    nro = atoi(&car); /*converto il carattere letto in un intero*/
                    somma+=(long int)nro; /*e lo aggiungo alla somma*/
                    trovati++; /*incremento numero di caratteri trovati*/
                }
            }

            /*al termine del ciclo di lettura*/
            write(piped[n][1], &somma, sizeof(somma)); /*figlio comunica al padre la somma calcolata*/
            
            exit(trovati);
        }
        
    }

    /* processo padre */
    //padre non scrive su nessuna pipe ma legge da tutte
    for(n = 0; n < N; n++){
        close(piped[n][1]);
    }

    for(n = 0; n < N; n++){ /*padre rispettando l'ordine dei file*/
        read(piped[n][0], &somma, sizeof(somma)); /*legge dalla pipe la somma comunicatagli dal figlio*/
        printf("Il figlio di indice %d associato al file %s ha comunicato %ld\n", n, argv[n+1], somma);
    }

    /*padre aspetta i figli*/
    for(n = 0; n < N; n++){
        
        
        if ((pid = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(5);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Il figlio con pid %d ha ritornato %d (se 255 problemi)\n", pid, ritorno);
        }
    }



    exit(0);
}