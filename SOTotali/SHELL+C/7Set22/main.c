#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#define PERM 0644
//definisco il tipo pipe_t
typedef int pipe_t[2];
int main(int argc, char** argv) {
    
    char car; /*carattere letto correntemente dai figli dal proprio file*/
    char last; /*ultimo carattere dispari letto dal file dal figlio*/
    char *cur; /*array da passare fra i vari figli ed il padre*/
    int N; /*numero di processi figli*/
    int n; /*indice dei processi figli*/
    int j; /*indice per cicli*/
    int fcreato; /*file descriptor del file creato*/
    int pid;	// memorizza il valore di ritorno della funzione fork
    int fd; // variabile che conterra' il file descriptor del file aperto dal figlio
    int nr; /*variabile per valore di ritorno della read*/
    int nw; /*variabile per valore di ritorno della write*/
    int L; /*lunghezza (dispari) dei file passati*/
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    pipe_t* piped; /*pipe di comunicazione in pipeline fra i figli, semantica: si legge da pipe n e si scrive su pipe (n+1)%N in modo che l'ultimo figlio scriva sulla pipe 0 che verrà letta dal padre*/

    if (argc < 3) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    /*calcolo il numero di file passati*/
    N = argc - 1;

    //creo il file "NAPOLITANO"
    if((fcreato = creat("NAPOLITANO", PERM)) < 0){		/* ERRORE se non si riesce a creare il file */
        printf("Errore in creazione del file %s dato che fcreato = %d\n", "NAPOLITANO", fcreato);
        exit(2);
    }

    /*allocazione della memoria per l'array di dimensione N da passare fra i vari figli ed il padre*/
    cur = (char *)malloc(N * sizeof(char));
    if(cur == NULL){
        printf("Errore in malloc\n");
        exit(3);
    }

    //allocazione della memoria per l'array dinamico di N pipes
    piped = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(4);
    }
    /*creazione delle N pipes*/
    for(n = 0; n < N; n++){
        if(pipe(piped[n])<0){
            printf("Errore nella creazione della pipe numero:%d\n",n);
            exit(5);
        }
    }

    //apro il file argv[1]
    if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
        printf("Errore in apertura file %s dato che f = %d\n", argv[1], fd);
        exit(6);
    }

    L = lseek(fd, 0L, SEEK_END); /*calcolo la lunghezza del primo file (è la stessa per tutti i file)*/
    close(fd); /*padre chiude il file siccome non lo usa più*/

    /*Ciclo di generazione degli N figli*/
    for(n = 0; n < N; n++){
        
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(7);
        }
        
        if (pid == 0)
        {	/* processo figlio */

            //figlio legge solo dalla pipe di indice n e scrive solo sulla pipe di indice (n+1)%N
            for(j = 0; j < N; j++){
                if(j != n){
                    close(piped[j][0]);
                }
                
                if (j != (n+1)%N) {
                    close(piped[j][1]);
                }
            }
            //apro il file argv[n+1]
            
            if((fd = open(argv[n+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[n+1], fd);
                exit(-1); /*in caso di errore si decide di tornare -1 che sarà interpretato come 255 che non è un valore ammissibile*/
            }

            for(j = 0; j < L; j++){ /*figlio legge tutti i caratteri del file*/
                    read(fd, &car, sizeof(char)); /*leggo un carattere dal file*/
                    if(j%2 != 0){ /*se il carattere letto è in posizione dispari*/
                        last = car; /*salvo il carattere*/
                        if(n != 0){ /*se non è il primo figlio*/
                        nr = read(piped[n][0], cur, N); /*legge dalla pipe di indice n l'array di caratteri dispari*/
                        if(nr != N){ /*controllo che la lettura sia andata a buon fine*/
                            printf("Il figlio %d ha letto un numero di byte sbagliati %i\n", n, nr);
                            exit(-1);
                        }
                    }
                    cur[n] = car; /*figlio scrive il carattere sull'array*/
                    nw = write(piped[(n+1)%N][1], cur, N); /*e invia l'array di caratteri dispari al figlio successivo*/
                    if(nw != N){
                        /*controllo che la scrittura sia andata a buon fine*/
                        printf("Il figlio %d ha scritto un numero di byte sbagliati %i\n", n, nw);
                        exit(-1);
                    }
                }                
            }
            /*al termine del ciclo di lettura il figlio ritorna al padre l'ultimo carattere dispari letto*/
            exit(last);
        }
       
        
    }

    /* processo padre */
    //padre non scrive su nessuna pipe e legge dalla pipe 0
    for(n = 0; n < N; n++){
        close(piped[n][1]);
        if (n != 0) {
            close(piped[n][0]);
        }
    }

    for(j = 1; j < L; j+=2){ /*per ognuno dei caratteri dispari*/
        nr = read(piped[0][0], cur, N); /*padre legge l'array di caratteri dispari*/
        if(nr != N){ /*se ho avuto problemi nella lettura*/
            printf("Il padre ha letto un numero di byte sbagliati %i\n", nr);
            exit(8);
        }
        nw = write(fcreato, cur, N); /*padre scrive array di caratteri sul file creato*/
        if(nw != N){ /*se ho avuto problemi nella scrittura*/
            printf("Il padre ha scritto un numero di byte sbagliati %i\n", nw);
            exit(9);
        }
    }

    /*padre aspetta i figli*/
    for(n = 0; n < N; n++){
        
        if ((pid = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(10);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Il figlio con pid %d ha ritornato %d (che corrisponde al carattere '%c')\n", pid, ritorno, (char)ritorno);
        }
    }

    exit(0);
}