#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <ctype.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];
int main(int argc, char** argv) {

    char c; /*carattere letto*/
    int N; /*numero di figli da generare*/
    int i = 0; /*contatore*/
    int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
    int fd; /*variabile che conterra il file descriptor del file che stiamo per aprire */
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    int k; /*contatore*/
    long int t; /*trasformazioni effettuate*/
    long int tNipote; /*variabile in cui il padre memorizzerà le trasformazioni effettuate dal nipote*/
    pipe_t* pipedFP; /*pipe per la comunicazione figli-padre*/
    pipe_t* pipedNP; /*pipe per la comunicazione nipoti-padre*/
    
    if (argc < 3) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    N = argc - 1;

    //apro N pipes
    pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (pipedFP == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(2);
    }
    
    for(int i = 0;i < N;i++){
        if(pipe(pipedFP[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(3);
        }
    }

    //apro N pipes
    pipedNP = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (pipedNP == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(4);
    }
    
    for(int i = 0;i < N;i++){
        if(pipe(pipedNP[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(5);
        }
    }
    for(i=0; i<N; i++){
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(5);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            t = 0L;
            
            if ((pidFiglio = fork()) < 0)	/* Il processo figlio crea un nipote */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(-1);
            }
            
            if (pidFiglio == 0)
            {	/* processo nipote */
                //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
                for(k = 0; k < N; k++){
                    close(pipedFP[k][1]);
                    close(pipedFP[k][0]);
                    close(pipedNP[k][0]);
                    if (k != i) {
                        close(pipedNP[k][1]);
                    }
                }
                t = 0L;
                //controllo se il file e' accedibile
                fd = 0;
                if((fd = open(argv[i+1], O_RDWR)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                    printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                    exit(-1);
                }

                while(read(fd, &c, sizeof(char))){
                    if(isalpha(c) && islower(c)){ /*se il carattere letto è alfabetico minuscolo*/
                        lseek(fd, -1, SEEK_CUR); /*mi rimetto col cursore sul carattere appena letto*/
                        c = toupper(c);
                        write(fd, &c, sizeof(char)); /*lo sostituisco con la corrispondente versione maiuscola*/
                        t++;
                        printf("DEBUG:NIPOTE HA SCRITTO\n");
                    }
                }
                write(pipedNP[i][1], &t, sizeof(t)); /*nipote comunica al padre numero di trasformazioni effettuate*/
                exit(t/256);
            }
            /* processo figlio */
            //controllo se il file e' accedibile
            fd = 0;
            if((fd = open(argv[i+1], O_RDWR)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(-1);
            }
            //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
            for(k = 0; k < N; k++){
                close(pipedFP[k][0]);
                if (k != i) {
                    close(pipedFP[k][1]);
                }
                close(pipedNP[k][1]);
                close(pipedNP[k][0]);
            }
            while (read(fd, &c, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(isdigit(c)){ /*se il carattere letto è un numero*/
                    lseek(fd, -1, SEEK_CUR); /*mi rimetto col cursore sul carattere appena letto*/
                    c = ' ';
                    write(fd, &c, sizeof(char)); /*lo sostituisco con lo spazio*/
                    t++;
                    printf("DEBUG:FIGLIO HA SCRITTO\n");
                }
            }
            write(pipedFP[i][1], &t, sizeof(t)); /*figlio comunica al padre numero di trasformazioni effettuate*/

            /*figlio aspetta il nipote*/
            if ((pidFiglio = wait(&status)) < 0) {
                printf("Non e' stato creato nessun processo figlio\n");
                exit(-1);
            }
            
            if ((status & 0xFF) != 0)
            {
                printf("Il processo figlio è stato terminato in modo anomalo\n");
            } else {
                ritorno = (status >> 8) & 0xFF;
                printf("Il nipote con pid %d ha ritornato %d\n", pidFiglio, ritorno);
            }
            exit(t/256);
        }
        /* processo padre */
    }

    //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
    for(k = 0; k < N; k++){
        close(pipedFP[k][1]);
        close(pipedNP[k][1]);
    }
    for(i = 0; i<N; i++){
        printf("DEBUG:PADRE INIZIA LETTURA\n");
        read(pipedFP[i][0], &t, sizeof(t));
        read(pipedNP[i][0], &tNipote, sizeof(tNipote));
        printf("DEBUG:PADRE HA LETTO\n");
        printf("Dal file %s il figlio ha trovato %ld occorrenze di caratteri numerici e il nipote ha trovato %ld occorrenze di caratteri alfabetici minuscoli\n", argv[i+1], t, tNipote);
    }

    /*padre aspetta i figli*/
    for(i=0; i<N; i++){
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(6);
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