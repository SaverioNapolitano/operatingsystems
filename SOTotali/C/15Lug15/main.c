#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <signal.h>
#define PERM 0644

//definisco il tipo pipe_t
typedef int pipe_t[2];
int main(int argc, char** argv) {
    
    char ch;	// carattere usato per leggere il contenuto del file
    char ok; /*variabile che verrà usata dal padre per sincronizzare i figli (non importa che valore abbia)*/
    int N; /*numero di file*/
    int fcreato; // variabile che conterra il file descriptor del file "Merge" */
    int i; /*indice per cicli*/
    int j; /*indice per cicli*/
    int *pid;	// array dinamico per memorizzare i pid dei figli generati con la fork
    int fd; // variabile che conterra' il file descriptor del file argv[i+1] che verrà aperto dal figlio con la open
    int finito; /*variabile che vale 0 se nessun figlio è finito e 1 se un figlio è terminato*/
    int nr; /*per valore di ritorno della read*/
    int nw; /*per valore di ritorno della write*/
    int pidFiglio;
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    pipe_t* pipedPF; /*pipe per la comunicazione fra padre e figli*/
    pipe_t* pipedFP; /*pipe per la comunicazione fra figli e padre*/
    
    if (argc < 3) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    /*calcoliamo il numero di file passati*/
    N = argc - 1;

    //creo il file "Merge"
    
    if((fcreato = creat("Merge", PERM)) < 0){		/* ERRORE se non si riesce a creare il file */
        printf("Errore in creazione del file %s dato che fd = %d\n", "Merge", fcreato);
        exit(2);
    }

    //allocazione della memoria per l'array dinamico di N pipes per la comunicazione fra padre e figli
    pipedPF = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    //allocazione della memoria per l'array dinamico di N pipes per la comunicazione fra figli e padre
    pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (pipedPF == NULL || pipedFP == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(3);
    }
    
    /*creazione delle N pipes padre-figli e delle N pipes figli-padre*/
    for(i = 0; i < N; i++){
        if(pipe(pipedPF[i])<0){
            printf("Errore nella creazione della pipe numero:%d\n",i);
            exit(4);
        }
        if(pipe(pipedFP[i])<0){
            printf("Errore nella creazione della pipe numero:%d\n",i);
            exit(5);
        }
    }
    /*allocazione della memoria per l'array dinamico dei pid*/
    pid = (int *)malloc(N*sizeof(int));
    if(pid == NULL){
        printf("Errore in malloc\n");
        exit(6);
    }
    /*ciclo di generazione degli N figli*/
    for(i = 0; i < N; i++){
        
        
        if ((pid[i] = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(7);
        }
        
        if (pid[i] == 0)
        {	/* processo figlio */
            //figlio legge solo dalla pipe padre-figlio di indice i e scrive solo sulla pipe figlio-padre di indice i
            for(j = 0; j < N; j++){
                close(pipedPF[j][1]);
                close(pipedFP[j][0]);
                if (j != i) {
                    close(pipedPF[j][0]);
                    close(pipedFP[j][1]);
                }
            }
            //apro il file argv[i+1]
            
            if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(-1);
            }

            nr = read(pipedPF[i][0], &ok, sizeof(char)); /*figlio aspetta che il padre gli comunichi che può procedere*/
            if(nr != sizeof(char)){
                printf("Figlio di indice %d ha letto numero di byte sbagliati dalla pipe %i\n", i, nr);
                exit(-1);
            }
            
            
            while (read(fd, &ch, sizeof(char)))	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                nw = write(pipedFP[i][1], &ch, sizeof(char)); /*figlio comunica al padre il carattere letto dal file*/
                if(nw != sizeof(char)){
                    printf("Figlio di indice %d ha scritto numero di byte sbagliati sulla pipe %i\n", i, nw);
                    exit(-1);
                }
                nr = read(pipedPF[i][0], &ok, sizeof(char)); /*e aspetta che il padre gli comunichi che può procedere*/
                if(nr != sizeof(char)){
                    printf("Figlio di indice %d ha letto numero di byte sbagliati dalla pipe %i\n", i, nr);
                    exit(-1);
                }   
            }

            /*al termine dell'esecuzione il processo associato al file più corto ritorna al padre l'ultimo carattere letto*/
            exit(ch);
        }
    }

    /* processo padre */
    //padre legge da tutte le pipe figli-padre e scrive su tutte le pipe padre-figli
    for(i = 0; i < N; i++){
        close(pipedFP[i][1]);
        close(pipedPF[i][0]);
    }

    finito = 0; /*all'inizio si suppone non sia terminato nessun figlio*/
    while(!finito){ /*fino a quando nessun figlio è terminato*/
        for(i = 0; i < N; i++){ /*per ogni figlio*/
            nw = write(pipedPF[i][1], &ok, sizeof(char)); /*padre comunica al figlio di indice i che può procedere*/
            if(nw != sizeof(char)){
                printf("Padre ha scritto numero di byte sbagliati sulla pipe %i\n", nw);
                exit(-1);
            }
            nr = read(pipedFP[i][0], &ch, sizeof(char)); /*padre legge dalla pipe il carattere comunicato dal figlio*/
            if(nr != sizeof(char)){
                finito = 1; /*se ho avuto problemi nella lettura aggiorno il valore di finito*/
                j = i; /*memorizzo l'indice del processo che è terminato*/
                break; /*ed esco dal ciclo*/
            }
            nw = write(fcreato, &ch, sizeof(char)); /*padre scrive sul file creato il carattere letto dal figlio*/
            if(nw != sizeof(char)){
                printf("Padre ha scritto numero di byte sbagliati sul file %i\n", nw);
                exit(-1);
            }
        }
    }

    /*se arrivo qua un figlio è terminato, padre deve inviare segnale per terminare forzatamente tutti gli altri*/
    for(i = 0; i < N; i++){
        if(i != j){ /*se il processo non è il figlio terminato normalmente*/
            kill(pid[i], SIGKILL); /*padre forza la terminazione con il segnale SIGKILL*/
        }
    }

    /*padre aspetta i figli*/
    for(i = 0; i < N; i++){ 
       
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(8);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio con pid %d è stato terminato in modo anomalo e ha ritornato %d\n", pidFiglio, status & 0xFF);
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Il figlio con pid %d è terminato in modo normale e ha ritornato %d che corrisponde al carattere %c\n", pidFiglio, ritorno, (char)ritorno);
            
        }
    }

    exit(0);
}