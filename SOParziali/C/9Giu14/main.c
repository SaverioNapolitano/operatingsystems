#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, exec, read, write
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <errno.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];
int main(int argc, char** argv) {
    
    int pidChild;
            int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
            int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    if (argc < 3) /* controllo sul numero di parasmetri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    //apro argc-1 pipes
    pipe_t* piped=malloc(sizeof(pipe_t)*(argc-1));
    for(int i=0;i<argc-1;i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i",i);
            exit(4);
        }
    }
    int lunghezza;
    long int totale;
    for(int i = 0; i < argc - 1; i++){
        int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(2);
        }
        if (pidFiglio == 0)
        {	/* processo figlio */
            //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
            for(int k = 0; k < argc-1; k++){
                close(piped[k][0]);
                if (k != i) {
                    close(piped[k][1]);
                }

            }
            //controllo se il file e' accedibile
            int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(1);
            }
            close(fd);
            int pidNipote;	// memorizzo il valore di ritorno della funzione fork
             int pipedFN[2];
            
            if (pipe(pipedFN) < 0 )
            {	printf("Errore creazione pipe\n");
                exit(3);
            }
            if ((pidNipote = fork()) < 0)	/* Il processo padre crea un figlio */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(2);
            }
            if (pidNipote == 0)
            {	/* processo nipote */
                close(piped[i][1]);
                close(0); //chiudo standard input
                fd = open(argv[i+1], O_RDONLY);
                close(1); //chiudo standard output
                dup(pipedFN[1]); //ora lo standard output del nipote è collegato alla pipe lato scrittura
                close(pipedFN[0]); 
                close(pipedFN[1]); //chiudo pipe siccome non serve più
                execlp("wc", "wc", "-l",(char *)0);
                /* si esegue l'istruzione seguente SOLO in caso di fallimento */
                perror("errore esecuzione wc");
                exit(errno);
            }
            /* processo figlio */

            close(pipedFN[1]);
            if ((pidChild = wait(&status)) < 0) {
                printf("Non e' stato creato nessun processo figlio\n");
                exit(9);
            }
            
            if ((status & 0xFF) != 0)
            {
                printf("%d\n", status & 0xFF);
                printf("Il processo nipote è stato terminato in modo anomalo\n");
            } else {
                ritorno = (status >> 8) & 0xFF;
            
            }
            char stringa[256];
            int j = 0;
            while(read(pipedFN[0], &(stringa[j]), 1)){ //fino a quando ci sono caratteri da leggere
                if(stringa[j]=='\n'){
                    stringa[j]=0;
                    break;
                }
                j++;
            }
            lunghezza = atoi(stringa);
            write(piped[i][1], &lunghezza, sizeof(lunghezza));
            printf("DEBUG:lunghezza=%i pipen=%i ad i=%i\n",lunghezza,piped[i][1],i);
            exit(ritorno);
        }
        /* processo padre */
    }
    //chiudo tutte le pipe in scrittura
    for(int k = 0; k < argc - 1; k++){
        close(piped[k][1]);
    }
    totale = 0;
    for(int i=0;i<argc-1;i++){
        read(piped[i][0], &lunghezza, sizeof(lunghezza));
        totale+=(long int)lunghezza;
        
    }
    printf("Totale=%ld\n", totale);
    
    for(int j = 0; j<argc-1;j++){
        if ((pidChild = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(7);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            
            printf("Figlio con pid=%d e valore di ritorno=%d\n", pidChild, ritorno);
        }
    }
    exit(0);
}