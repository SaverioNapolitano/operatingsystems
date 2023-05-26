#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, exec, read, write
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>

//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    
    char c1; /*primo carattere passato*/
    char c2; /*secondo carattere passato*/
    char c; /*carattere letto*/
    int i; /*contatore*/
    int k; /*contatore*/
    int N; /*numero di figli da generare*/
    int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
    int fd; /*variabile che conterra il file descriptor del file che verrà aperto con la open */
    int occorrenze; /*occorrenze del carattere da cercare*/
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    long int PC1; /*posizione primo carattere*/
    long int PC2; /*posizione secondo carattere*/
    pipe_t* pipedNP; /*pipe utilizzate dal nipote per comunicare col padre*/
    

    struct {
        char car; /*carattere trovato*/
        long int pos; /*posizione in cui è stato trovato*/
    } carpos;


    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    N = argc - 3;
    //controlla che la stringa <argv[argc-2]> sia un singolo carattere
    if (strlen(argv[argc-2]) != 1) {	
        printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc-2]);
        exit(2);
    }
    c1 = argv[argc-2][0];

    //controlla che la stringa <argv[argc-1]> sia un singolo carattere
    if (strlen(argv[argc-1]) != 1) {
        printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc-1]);
        exit(3);
    }
    c2 = argv[argc-1][0];

    //apro N pipes
    pipedNP = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (pipedNP == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(6);
    }
    
    for(i = 0;i < N;i++){
        if(pipe(pipedNP[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(7);
        }
    }

    for(i=0; i<N; i++){
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(8);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            occorrenze=0;
            //chiudo tutte le pipe in lettura e scrittura di padre e nipote, 
            
            pipe_t pipedFN;
            
            if (pipe(pipedFN) < 0 )
            {	printf("Errore creazione pipe\n");
                exit(-1);
            }

            //controllo se il file e' accedibile
            fd = 0;
            if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(-1);
            }
            if ((pidFiglio = fork()) < 0)	/* Il processo figlio crea un nipote */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(-1);
            }
            
            if (pidFiglio == 0)
            {	/* processo nipote */
                occorrenze = 0;
                //chiudo tutte le pipe in lettura e scrittura di padre e nipote tranne la i-sima aperta in scrittura
                //tutte le pipe in lettura e scrittura di figlio e nipote tranne la i-sima aperta in lettura
                for(k = 0; k < N; k++){
                    close(pipedNP[k][0]);
                    if (k != i) {
                        close(pipedNP[k][1]);
                    }
                }

                close(pipedFN[1]);

                fd = 0;
                if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                    printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                    exit(-1);
                }
                PC2=0;
                while(read(fd, &c, sizeof(char))){
                    if(c == c2){
                        occorrenze++;
                        read(pipedFN[0], &carpos, sizeof(carpos));
                        if(PC2<carpos.pos){
                            carpos.car=c2;
                            carpos.pos=PC2;
                        }
                        write(pipedNP[i][1], &carpos, sizeof(carpos));
                    }
                    PC2++;
                }

                exit(occorrenze);
            }
            /* processo figlio */
            for(k=0; k<N; k++){
                close(pipedNP[k][1]);
                close(pipedNP[k][0]);
            }
            PC1=0;
            while(read(fd, &c, sizeof(char))){
                if(c == c1){
                    occorrenze++;
                    carpos.car = c1;
                    carpos.pos = PC1;
                    write(pipedFN[1], &carpos, sizeof(carpos)); /*figlio comunica struct al nipote*/
                }
                PC1++;
            }
            
            if ((pidFiglio = wait(&status)) < 0) {
                printf("Non e' stato creato nessun processo figlio\n");
                exit(-1);
            }
            
            if ((status & 0xFF) != 0)
            {
                printf("Il processo nipote è stato terminato in modo anomalo\n");
            } else {
                ritorno = (status >> 8) & 0xFF;
                printf("Nipote con pid %d ha ritornato %d\n", pidFiglio, ritorno);
            }
            exit(occorrenze);
        }
        /* processo padre */
        
    }

    //chiudo tutte le pipe tra figlio e nipote in lettura e scrittura e tutte le pipe tra nipote e padre in scrittura
    for(int k = 0; k < N; k++){
        close(pipedNP[k][1]);
    }

    for(i=0; i<N; i++){
        while(read(pipedNP[i][0], &carpos, sizeof(carpos))){
            printf("Per il file %s il carattere %c si trova in posizione %ld\n", argv[i+1], carpos.car, carpos.pos);
        }
    }

    for(i=0; i<N; i++){
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(9);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo: segnale di terminazione = %d\n", status & 0xFF);
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Figlio con pid %d ha ritornato %d\n", pidFiglio, ritorno);
        }
    }

    exit(0);
}