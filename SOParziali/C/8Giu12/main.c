#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <errno.h>
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];
int main(int argc, char** argv) {

    char l[256]; /*array usato dal figlio per leggere da standard output la stringa scritta dal nipote*/
    char s[12];
    char s1[] = "Sopra media";
    char s2[] = "Sotto media";
    int N; /*numero di processi figli*/
    int H; /*lunghezza media in linee dei file*/
    int i; /*indice*/
    int j; /*indice*/
    int pid;// memorizzo il valore di ritorno della funzione fork
    int fd; /*variabile che conterra il file descriptor del file che verrà aperto con la open */
    int lunghezza; /*lunghezza del file calcolata dal figlio convertendo la stringa scritta dal nipote*/
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    pipe_t* piped; /*pipe per comunicazione padre-figli*/
    
    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    N = argc - 2;

    //controllo utlizzando la funzione atoi se il argv[argc-1] e un numero strettamente positivo
    H = atoi(argv[argc-1]);
    if(H <= 0){
        printf("il parametro %s non un numero positivo\n",argv[argc-1]);
        exit(2);
    }

    //apro Fasi pipes
    piped = (pipe_t*)malloc(sizeof(pipe_t) * N);
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(3);
    }
    
    for(i = 0;i < N;i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(4);
        }
    }

    for(i=0; i<N; i++){
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(5);
        }
        
        if (pid == 0)
        {	/* processo figlio */
            pipe_t pipedFN; /*pipe per la comunicazione figlio-nipote*/
            if (pipe(pipedFN) < 0 )
            {	printf("Errore creazione pipe\n");
                exit(-1);
            }
            //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
            for(j = 0; j < N; j++){
                close(piped[j][0]);
                if (j != i) {
                    close(piped[j][1]);
                }
            }
            if ((pid = fork()) < 0)	/* Il processo figlio crea un nipote */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(-1);
            }
            
            if (pid == 0)
            {	/* processo nipote */
                
                close(0); /*chiudo lo standard input del nipote così da poterlo ridirigere sul file*/
                //controllo se il file e' accedibile
                if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                    printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                    exit(-1);
                }
                close(1); /*chiudo lo standard output così da poterlo ridirigere sulla pipe*/
                //duplico la pipe lato scrittura
                dup(pipedFN[1]);
                //chiudo il lato della pipe duplicato
                close(pipedFN[1]);
                close(pipedFN[0]);
                
                /*nipote calcola lunghezza in linee del file associato*/
                execlp("wc", "wc", "-l", (char*)0);
                /* si esegue l'istruzione seguente SOLO in caso di fallimento */
                
                perror("errore esecuzione comando");
                exit(errno);
            }
            /* processo figlio */
            close(pipedFN[1]);
            /*legge da standard output ciò che ha scritto il nipote*/
            j = 0; /*inizalizzo indice stringa*/
            while(read(pipedFN[0], &(l[j]), sizeof(char))){ /*fino a quando ci sono caratteri da leggere dallo standard input*/
                if(l[j]=='\n'){ /*se trovo il terminatore di linea esco*/
                    break;
                }
                j++;
            }
            if(j != 0){ /*se ha letto qualcosa*/
                l[j]=0; /*al termine della lettura metto il terminatore di stringa*/
                lunghezza = atoi(l); /*e la converto in intero*/
                if(lunghezza>=H){ /*se la lunghezza è maggiore o uguale alla media la stringa da scrivere sarà s1*/
                    strcpy(s, s1);
                } else {
                    strcpy(s, s2); /*altrimenti s2*/
                }

                write(piped[i][1], s, sizeof(s)); /*figlio comunica al padre la stringa*/
            }
            

            /*figlio aspetta nipote*/
            if ((pid = wait(&status)) < 0) {
                printf("Non e' stato creato nessun processo figlio\n");
                exit(-1);
            }
            
            if ((status & 0xFF) != 0)
            {
                printf("Il figlio è terminato in modo anomalo\n");
                ritorno = status & 0xFF;
            } else {
                ritorno = (status >> 8) & 0xFF;
            }
            
            exit(ritorno);
        }
    }

    /* processo padre */

    //chiudo tutte le pipe in lettura
    for(i = 0; i < N; i++){
        close(piped[i][1]);
    }

    /*padre legge dalla pipe la stringa comunicata dal figlio*/
    for(i=0;i<N;i++){
        if(read(piped[i][0], s, sizeof(s))){
             printf("Per il file %s la stringa ricevuta corrisponde a '%s'\n", argv[i+1], s);
        }
    }

    /*padre aspetta i figli*/
    for(i=0; i<N;i++){
        if ((pid = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(6);
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