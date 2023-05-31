#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait

//definisco il tipo pipe_t
typedef int pipe_t[2];
int main(int argc, char** argv) {

    char s[256]; /*array di caratteri in cui verrà memorizzata la linea letta dal file*/
    int N; /*numero di processi figli*/
    int K; /*lunghezza in linee media dei file*/
    int X; /*numero richiesto all'utente che indica la linea da leggere*/
    int L; /*lunghezza della X-esima linea letta*/
    int numLinea; /*numero della linea corrente*/
    int i; /*indice*/
    int j; /*indice*/
    int pidFiglio; /*variabile per memorizzare valore di ritorno della wait*/
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figli
    int fd; /*variabile che conterra il file descriptor del file che verrà aperto con la open */
    int *pid;// array per memorizzare i pid di tutti i processi figli
    pipe_t* piped; /*pipe per la comunicazione padre-figli*/
    
    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    N = argc - 2;

    //controllo utlizzando la funzione atoi se il argv[argc-1] è un numero strettamente positivo
    if((K = atoi(argv[argc-1])) <= 0){
        printf("il parametro %s non un numero positivo\n",argv[argc-1]);
        exit(2);
    }

    do{
        printf("Inserire un numero fra 0 (escluso) e %d (incluso)\n", K);
        scanf("%d", &X);
    } while(X <= 0 || X > K);

    /*allocazione memoria per array dinamico di N pid*/
    pid = (int *)malloc(N*sizeof(int));
    if(pid == NULL){
        printf("Errore in malloc\n");
        exit(3);
    }
    //allocazione memoria per array dinamico di N pipe
    piped = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(4);
    }
    /*creazione delle N pipe*/
    for(int i = 0;i < N;i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(5);
        }
    }

    for(i = 0; i < N; i++){
        
        
        if ((pid[i] = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(6);
        }
        
        if (pid[i] == 0)
        {	/* processo figlio */
            //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
            for(j = 0; j < N; j++){
                close(piped[j][0]);
                if (j != i) {
                    close(piped[j][1]);
                }
            }
            //controllo se il file e' accedibile
            if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(-1);
            }

            numLinea = 1;
            j = 0;
            
            while (read(fd, &(s[j]), sizeof(char)))	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(s[j]=='\n'){ /*se ho trovato il terminatore di linea*/
                    if(numLinea==X){/*se la linea è quella richiesta dall'utente*/
                        L = j+1; /*memorizzo la sua lunghezza compreso il terminatore di linea*/
                        write(piped[i][1], &L, sizeof(L)); /*figlio comunica al padre la lunghezza della linea*/
                        write(piped[i][1], s, L*sizeof(char)); /*e quindi la linea stessa*/
                        break; /*ed esco dal ciclo*/
                    } else {
                        numLinea++; /*incremento il numero della linea*/
                    }
                    j = 0; /*inizializzo l'indice*/
                } else {
                    j++; /*incremento l'indice*/
                }
            }

            if(numLinea<X){/*se ho letto tutto il file senza aver trovato la linea richiesta*/
                L = 0; /*imposto la lunghezza a un valore nullo per far capire al padre che non l'ho trovata*/
                s[0] = 0; /*e imposto la stringa come stringa nulla*/
                /*comunico ugualmente i dati al padre*/
                write(piped[i][1], &L, sizeof(L));
                write(piped[i][1], s, L*sizeof(char)); 
            }
            exit(L);
        }
    }

    /* processo padre */
    //chiudo tutte le pipe in scrittura
    for(i = 0; i < N; i++){
        close(piped[i][1]);
    }

    for(i = 0; i < N; i++){
        read(piped[i][0], &L, sizeof(L)); /*padre legge dalla pipe la lunghezza della linea*/
        if(L > 0){ /*se la lunghezza è un numero positivo allora è stata trovata la linea*/
            read(piped[i][0], s, L*sizeof(char)); /*padre legge linea dalla pipe*/
            s[L-1]=0;
            printf("il figlio con pid %d ha letto la linea '%s' dal file %s\n", pid[i], s, argv[i+1]);
        }
    }

    /*padre aspetta i figli*/
    for(i = 0; i < N; i++){
        
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(7);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Il figlio con pid %d ha ritornato %d\n", pidFiglio, ritorno);
        }
    }

    exit(0);
}