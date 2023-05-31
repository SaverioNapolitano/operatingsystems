#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
#define PERM 0644
//definisco il tipo pipe_t
typedef int pipe_t[2];
int main(int argc, char** argv) {

    char *b; /*blocco corrente (B1 o B2) letto dai figli dal file*/
    char *Fcreato; /*stringa che rappresenterà il nome del file creato dal secondo processo della coppia*/
    int totale; /*lunghezza dei file in blocchi (la stessa per tutti)*/
    int N; /*numero di file*/
    int C; /*dimensione in byte dei blocchi*/
    int i; /*indice processi figli*/
    int j; /*indice per cicli*/
    int nro; /*numero di blocchi letti dalla propria metà del file*/
    int fcreato; /*file descriptor creato dal secondo processo di ogni coppia*/
    int pid;	// memorizza il valore di ritorno della funzione fork
    int fd; // variabile che conterra' il file descriptor del file che verrà aperto con la open dai figli
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    int nr; /*per valore di ritorno della read*/
    int nw; /*per valore di ritorno della write*/
    pipe_t* piped; /*pipe di comunicazione fra i due figli della coppia*/

    
    if (argc < 3) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 1*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    /*calcoliamo il numero di file passati*/
    N = argc - 2;

    
    //controllo utlizzando la funzione atoi se il argv[argc-1] e un numero strettamente positivo e se è dispari
    if((C = atoi(argv[argc-1])) <= 0 || (C%2 == 0)){
        printf("il parametro %s non un numero positivo o non dispari\n",argv[argc-1]);
        exit(2);
    }

    /*apro il primo file per calcolarne la lunghezza*/
    if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
        printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
        exit(3);
    }

    totale = lseek(fd, 0L, SEEK_END)/C; /*calcolo la lunghezza in blocchi di ogni file*/
    nro = totale/2; /*calcolo il numero di blocchi che ogni figlio della coppia dovrà leggere*/

    close(fd); /*padre chiude il file siccome non lo usa più*/

    /*allocazione di memoria per il blocco di C byte che verrà letto dai figli*/
    b = (char *)malloc(C*sizeof(char));
    if(b == NULL){
        printf("Errore in malloc\n");
        exit(4);
    }

    //allocazione della memoria per l'array dinamico di N pipes
    piped = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(5);
    }
    /*creazione delle N pipes*/
    for(i = 0; i < N; i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%d\n",i);
            exit(6);
        }
    }

    /*ciclo di generazione dei 2*N figli*/
    for(i = 0; i < 2*N; i++){
        
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(7);
        }
        
        if (pid == 0)
        {	/* processo figlio */
            if(i < N){ /*primo processo della coppia*/
                //primo figlio della coppia non legge da nessuna pipe e scrive solo sulla pipe di indice i
                for(j = 0; j < N; j++){
                    close(piped[j][0]);
                    if (j != i) {
                        close(piped[j][1]);
                    }
                }
                /*apro il file associato*/
                if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
                    printf("PRIMO: Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                    exit(-1);
                }
                for(j = 0; j < nro; j++){/*devo leggere esattamente nro blocchi della prima metà del file*/
                    nr = read(fd, b, C); /*figlio primo della coppia legge il suo blocco*/
                    if(nr != C){
                        printf("il figlio primo della coppia %d ha letto un numero di byte sbagliati %i dal file passato\n", i, nr);
                    }
                    nw = write(piped[i][1], b, C); /*lo comunica al secondo figlio della coppia*/
                    if(nw != C){
                        printf("Il figlio primo della coppia %d ha scritto un numero di byte sbagliati %i sulla pipe\n", i, nw);
                    }
                }

            } else { /*secondo processo della coppia*/
                //secondo figlio della coppia non scrive su nessuna pipe e legge solo dalla pipe di indice i
                for(j = 0; j < N; j++){
                    close(piped[j][1]);
                    if (j != i-N) {
                        close(piped[j][0]);
                    }
                }
                //apro il file argv[i-N+1]
                if((fd = open(argv[i-N+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
                    printf("SECONDO: Errore in apertura file %s dato che fd = %d\n", argv[i-N+1], fd);
                    exit(-1);
                }
                
                Fcreato = (char *)malloc(strlen(argv[i-N+1])+11); /*alloco la memoria necessaria per il nome del file da creare che deve risultare dalla concatenazione 
                con il nome del file in posizione i-N+1 (con i >= N) che trovo con strlen(argv[i+N-1]) (escluso il terminatore) + il punto '.' (1) + la parola "mescolato" (9)
                + il terminatore di stringa (1) = 11*/
                if(Fcreato == NULL){
                    printf("Errore in malloc\n");
                    exit(-1);
                }
                strcpy(Fcreato, argv[i-N+1]); /*copio nella stringa finale il nome del file*/
                strcat(Fcreato, ".mescolato");/*e lo concateno con la terminazione ".mescolato"*/
                //creo il file Fcreato
                if((fcreato = creat(Fcreato, PERM)) < 0){		/* ERRORE se non si riesce a creare il file */
                    printf("Errore in creazione del file %s dato che fd = %d\n", Fcreato, fd);
                    exit(-1);
                }
                
                lseek(fd, (long)nro*C, SEEK_SET); /*sposto il file pointer a metà del file in modo da leggere solo la seconda metà*/
                for(j = 0; j < nro; j++){ /*devo leggere esattamente nro blocchi della seconda metà del file*/
                    nr = read(fd, b, C); /*figlio secondo della coppia legge il suo blocco*/
                    if(nr != C){
                        printf("Il figlio secondo della coppia %d ha letto un numero di byte sbagliati %i dal file passato\n", i, nr);
                    }
                    nw = write(fcreato, b, C); /*e lo scrive sul file creato*/
                    if(nw != C){
                        printf("Il figlio secondo della coppia %d ha scritto un numero di byte sbagliati %i sul file creato\n", i, nw);
                    }
                    nr = read(piped[i-N][0], b, C); /*legge dalla pipe il blocco dati inviato dal primo figlio della coppia*/
                    if(nr != C){
                        printf("Il figlio secondo della coppia %d ha letto un numero di byte sbagliati %i dalla pipe\n", i, nr);
                    }
                    nw = write(fcreato, b, C); /*e lo scrive sul file creato*/
                    if(nw != C){
                        printf("Il figlio secondo della coppia %d ha scritto un numero di byte sbagliati %i sul file creato\n", i, nw);
                    }
                }
            }
            /*al termine dell'esecuzione ogni figlio della coppia ritorna al padre il numero di blocchi letti*/
            exit(nro);
        }
    }

    /* processo padre */
    //padre non legge e non scrive su nessuna pipe
    for(i = 0; i < N; i++){
        close(piped[i][1]);
        close(piped[i][1]);
    }

    /*padre aspetta i figli*/
    for(i = 0; i < 2*N; i++){
        if ((pid = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(8);
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