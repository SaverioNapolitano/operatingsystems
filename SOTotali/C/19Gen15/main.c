#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];
typedef struct {
    int pid;
    long int occmax;
    long int occtotale;
} S;

int main(int argc, char** argv) {

    char CX;	// carattere usato per leggere il contenuto del file
    char C1;    /*carattere che devono cercare i primi N figli*/
    char C2;    /*carattere che devono cercare i secondi N figli*/
    int M; /*numero file totale (2N)*/
    int N;      /*numero di figli da creare in ogni ciclo (M/2)*/
    int i;      /*indice per i cicli*/
    int j;      /*indice per i cicli*/
    int pid;	// memorizza il valore di ritorno della funzione fork
    int fd; // variabile che conterra' il file descriptor del file argv[i+1]
    int pidFiglio; /*per wait*/
    long int occ; /*occorrenze del carattere calcolate da ogni figlio*/
    S S1; /*struttura dati che verrà mandata al padre dal figlio di indice 0 dei primi N figli*/
    S S2; /*struttura dati che verrà mandata al padre dal figlio di indice 0 dei secondi N figli*/
    pipe_t* pipedFP1; /*pipe per comunicazione primi N figli-padre, leggo da i+1, scrivo su i*/
    pipe_t* pipedFP2; /*pipe per comunicazione secondi N figli-padre, leggo da i+1, scrivo su i*/

    if (argc < 4 + 1) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 4*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    /*calcolo il numero totale (2N) di figli da generare*/
    M = argc - 3;
    printf("DEBUG:2N = %d\n", M);

    if(M%2 != 0 || M < 2){ /*se non sono in numero pari o sono meno di due*/
        printf("Errore nel numero di parametri passati\n");
        exit(2);
    }

    N = M/2;
    printf("DEBUG:N = %d\n", N);
    //controlla che la stringa argv[argc-1] contenga un singolo carattere
    if (strlen(argv[argc-2]) != 1) {	
        printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc-1]);
        exit(3);
    }
    C1 = argv[argc-2][0];	// Memorizzo il singolo carattere nella variabile C1

    //controlla che la stringa argv[argc-1] contenga un singolo carattere
    if (strlen(argv[argc-1]) != 1) {
        printf("Errore, la stringa %s non e' un singolo carattere\n", argv[argc-1]);
        exit(4);
    }
    C2 = argv[argc-1][0];	// Memorizzo il singolo carattere nella variabile C2

    //allocazione della memoria per array dinamico di N pipes
    pipedFP1 = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (pipedFP1 == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(5);
    }
    /*creazione delle N pipes*/
    for(i = 0; i < N; i++){
        if(pipe(pipedFP1[i])<0){
            printf("Errore nella creazione della pipe numero:%d\n",i);
            exit(6);
        }
    }

    //allocazione della memoria per array dinamico di N pipes
    pipedFP2 = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (pipedFP2 == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(7);
    }
    /*creazione delle N pipes*/
    for(i = 0; i < N; i++){
        if(pipe(pipedFP2[i])<0){
            printf("Errore nella creazione della pipe numero:%d\n",i);
            exit(8);
        }
    }
    /*ciclo di generazione dei primi N figli*/
    for(i = 0; i < N; i++){
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(9);
        }
        
        if (pid == 0)
        {	/* processo figlio */
            //figlio scrive solo sulla pipe di indice i e legge solo dalla pipe di indice i+1 (tranne l'ultimo figlio che non legge da nessuno)
            for(j = 0; j < N; j++){
                if(j != i){
                    close(pipedFP1[j][1]);
                }
                if (j != i+1 || i == N-1) {
                    close(pipedFP1[j][0]);
                }
            }
            //apro il file argv[i+1]
            
            if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(-1);
            }

            occ = 0L; /*inizializzo il contatore delle occorrenze*/
            
            
            while (read(fd, &CX, sizeof(char)))	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(CX == C1){ /*se il carattere letto corrisponde a quello cercato*/
                    occ++; /*incremento il contatore delle occorrenze*/
                }
            }
            printf("DEBUG:primo figlio di indice %d e pid %d ha trovato %ld occorrenze del carattere %c nel file %s\n", i, getpid(), occ, C1, argv[i+1]);
            if(i == N-1){ /*se sono nell'ultimo figlio inizializzo la struttura che dovrà essere comunicata*/
                S1.pid = getpid();
                S1.occmax = occ;
                S1.occtotale = occ;
            } else { /*altrimenti leggo la struttura dalla pipe di indice i + 1*/
                read(pipedFP1[i+1][0], &S1, sizeof(S));
                if(S1.occmax < occ){ /*se le occorrenze del carattere trovate dal figlio corrente sono maggiori di quelle memorizzate nella struct*/
                    S1.pid = getpid(); /*aggiorno il pid del processo che ha calcolato il massimo numero di occorrenze*/
                    S1.occmax = occ; /*aggiorno il massimo numero di occorrenze*/
                }
                S1.occtotale+=occ; /*devo comunque sempre aggiornare il numero totale delle occorrenze*/
            }

            write(pipedFP1[i][1], &S1, sizeof(S)); /*comunico al figlio precedente la nuova struttura dati*/

            exit(0);
        }
    }
    
    /* processo padre */
    /*ciclo di generazione dei secondi N figli*/
    for(i = 0; i < N; i++){
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(10);
        }
        
        if (pid == 0)
        {	/* processo figlio */
            //figlio scrive solo sulla pipe di indice i e legge solo dalla pipe di indice i+1 (tranne l'ultimo figlio che non legge da nessuno)
            for(j = 0; j < N; j++){
                if(j != i){
                    close(pipedFP2[j][1]);
                }
                if (j != i+1 || i == N-1) {
                    close(pipedFP2[j][0]);
                }
            }
            //apro il file argv[i+N+1]
            
            if((fd = open(argv[i+N+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+N+1], fd);
                exit(-1);
            }

            occ = 0L;
            
            
            while (read(fd, &CX, sizeof(char)))	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(CX == C2){ /*se il carattere letto corrisponde a quello cercato*/
                    occ++; /*incremento il contatore delle occorrenze*/
                }
            }
            printf("DEBUG:secondo figlio di indice %d e pid %d ha trovato %ld occorrenze del carattere %c nel file %s\n", i, getpid(), occ, C2, argv[i+N+1]);
            if(i == N-1){ /*se sono nell'ultimo figlio inizializzo la struttura che dovrà essere comunicata*/
                S2.pid = getpid();
                S2.occmax = occ;
                S2.occtotale = occ;
            } else { /*altrimenti leggo la struttura dalla pipe di indice i + 1*/
                read(pipedFP2[i+1][0], &S2, sizeof(S));
                if(S2.occmax < occ){ /*se le occorrenze del carattere trovate dal figlio corrente sono maggiori di quelle memorizzate nella struct*/
                    S2.pid = getpid(); /*aggiorno il pid del processo che ha calcolato il massimo numero di occorrenze*/
                    S2.occmax = occ; /*aggiorno il massimo numero di occorrenze*/
                }
                S2.occtotale+=occ; /*devo comunque sempre aggiornare il numero totale delle occorrenze*/
            }

            write(pipedFP2[i][1], &S2, sizeof(S)); /*comunico al figlio precedente la nuova struttura dati*/

            exit(0);
        }
    }

    //padre deve leggere solo dalle due pipe di indice 0 
    for(i = 0; i < N; i++){
        close(pipedFP1[i][1]);
        close(pipedFP2[i][1]);
        if(i != 0){
            close(pipedFP1[i][0]);
            close(pipedFP2[i][0]);
        }
    }

    read(pipedFP1[0][0], &S1, sizeof(S)); /*padre legge la prima struttura dati*/
    printf("Il figlio con pid %d ha trovato %ld occorrenze del carattere %c mentre le occorrenze totali sono %ld\n", S1.pid, S1.occmax, C1, S1.occtotale);
    read(pipedFP2[0][0], &S2, sizeof(S)); /*padre legge la seconda struttura dati*/
    printf("Il figlio con pid %d ha trovato %ld occorrenze del carattere %c mentre le occorrenze totali sono %ld\n", S2.pid, S2.occmax, C2, S2.occtotale);
    
    /*padre aspetta tutti i figli (2N) disinteressandosi del valore di ritorno */
    for(i = 0; i < N; i++){
        
        if ((pidFiglio = wait((int*) 0)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(11);
        }
    }
    exit(0);
}