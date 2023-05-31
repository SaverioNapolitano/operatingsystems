#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {

    char Cx; /*carattere da cercare*/
    char c; /*carattere letto*/
    int N; /*numero di figli che verrano generati con la fork*/
    int i; /*contatore*/
    int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
    int fd; /*variabile che conterra il file descriptor del file che verrà aperto con la open */
    int finito; /*variabile che vale 1 se tutti i figli sono finiti, 0 se nessun figlio è finito*/
    int sostituzioni; /*numero di sostituzioni effettuate*/
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    long int pos; /*posizione del carattere trovato, il primo carattere del file viene considerato in posizione 0*/
    pipe_t* piped_fp; /*pipe per la comunicazione figli-padre*/
    pipe_t* piped_pf; /*pipe per la comunicazione padre-figli*/

    if (argc < 3) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 1*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    N = argc - 2;

    //controlla che la stringa argv[argc-1] sia un singolo carattere
    if (strlen(argv[argc-1]) != 1) {
        printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc-1]);
        exit(2);
    }
    Cx = argv[argc-1][0];

    //apro N pipes
    piped_fp = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (piped_fp == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(3);
    }
    
    for(int i = 0;i < N;i++){
        if(pipe(piped_fp[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(4);
        }
    }

    //apro N pipes
    piped_pf = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (piped_pf == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(5);
    }
    
    for(int i = 0;i < N;i++){
        if(pipe(piped_pf[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(6);
        }
    }

    for(i=0; i<N; i++){
        
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(7);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            sostituzioni=0;
            /*chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura per le pipe in cui il figlio scrive
            chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura per le pipe in cui il figlio legge */
            for(int k = 0; k < N; k++){
                close(piped_fp[k][0]);
                close(piped_pf[k][1]);
                if (k != i) {
                    close(piped_fp[k][1]);
                    close(piped_pf[k][0]);
                }
            }
            //controllo se il file e' accedibile
            fd = 0;
            if((fd = open(argv[i+1], O_RDWR)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA e SCRITTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(-1);
            }

            pos = 0L;
            while (read(fd, &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(c==Cx){ /*se il carattere letto corrisponde a quello da cercare*/
                    write(piped_fp[i][1], &pos, sizeof(pos)); /*figlio comunica al padre la posizione del carattere*/
                    read(piped_pf[i][0], &c, sizeof(char)); /*figlio riceve carattere dal padre*/
                    if(c!='\n'){/*se il carattere non è un a capo*/
                        lseek(fd, -1, SEEK_CUR); /*riposiziono il file pointer*/
                        write(fd, &c, sizeof(char)); /*sostituisco il carattere nel file*/
                        sostituzioni++;
                    }
                }
                pos++;
            }
            exit(sostituzioni);
        }
        /* processo padre */
        
    }
    //chiudo tutte le pipe dove scrive il figlio in scrittura e tutte le pipe dove scrive il padre in lettura
    for(int k = 0; k < N; k++){
        close(piped_fp[k][1]);
        close(piped_pf[k][0]);
    }

    finito = 0;
    while(!finito){
        finito = 1;
        for(i=0; i<N; i++){
            if((read(piped_fp[i][0], &pos, sizeof(pos)) == sizeof(pos))){/*se padre ha letto correttamente la posizione comunicata dal figlio*/
                printf("Il figlio di indice %d ha trovato in posizione %ld una occorrenza del carattere cercato nel file %s\n", i, pos, argv[i+1]);
                printf("Inserire un carattere per la sostituzione o invio per non sostituire\n");
                read(0, &c, sizeof(char)); /*il padre legge il carattere inserito dall'utente*/
                if(c!='\n'){
                    read(0, &Cx, sizeof(char));
                }
                write(piped_pf[i][1], &c, sizeof(char)); /*e lo comunica al figlio*/
                finito = 0; /*ho trovato almeno un figlio ancora in esecuzione*/
            }
        }
    }

    for(i=0; i<N; i++){
        
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(8);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Il figlio con pid %d ha ritornato %d (se 255 problemi)\n", pidFiglio, ritorno);
        }
    }

    exit(0);
}