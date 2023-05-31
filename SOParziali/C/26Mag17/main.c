#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>

//definisco il tipo pipe_t
typedef int pipe_t[2];
int main(int argc, char** argv) {

    char c; /*carattere letto*/
    char Cx; /*carattere da cercare*/
    int N; /*numero di processi figli da generare con la fork*/
    int i; /*indice*/
    int k; /*indice*/
    int *pid;	// array per memorizzare i pid dei processi figli generati dalla fork
    int fd; /*variabile che conterra il file descriptor del file che verrà aperto con la open */
    int pidFiglio; /*memorizza il valore di ritorno della wait*/
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    int nr; /*variabile che memorizza il valore di ritorno della read*/
    int nw; /*variabile che memorizza il valore di ritorno della write*/
    long int occ; /*occorrenze del carattere Cx nel file*/
    pipe_t* piped; /*pipe per la comunicazione in pipeline fra i figli: si legge dalla pipe i-esima e si scrive sulla i+1-esima*/

    struct {
        long int c1; /*valore massimo di occorrenze calcolate dal processo corrispondente*/
        int c2; /*indice d'ordine del processo che ha calcolato il massimo */
        long int c3; /*somma di tutte le occorrenze calcolate dai processi*/
    } S;
    
    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
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

    //allocazione di memoria per l'array dinamico di N pipe
    piped = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(3);
    }
    /*creazione delle N pipe*/
    for(i = 0;i < N;i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(4);
        }
    }

    /*allocazione della memoria per gli N pid dei figli*/
    pid = (int *)malloc(N * sizeof(int));
    if(pid == NULL){
        printf("Errore in malloc\n");
        exit(5);
    }
    
    S.c1 = -1; /*inizializzo contatore occorrenze singolo processo*/
    S.c3 = 0; /*inizializzo il contatore delle occorrenze totali*/
    for(i = 0; i < N; i++){
        
        
        if ((pid[i] = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(6);
        }
        
        if (pid[i] == 0)
        {	/* processo figlio */
            //chiudo tutte le pipe in lettura e scrittura tranne quella di indice i (aperta in lettura) e quella di indice i+1 (aperta in scrittura)
            for(k = 0; k < N; k++){
                if(k != i){
                    close(piped[k][0]);
                }
                if (k != (i+1)%N) {
                    close(piped[k][1]);
                }
            }
            //controllo se il file e' accedibile
            if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(-1);
            }

            
            occ = 0L; /*inizializzo il contatore delle occorrenze*/
            while (read(fd, &c, sizeof(char)))	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(c==Cx){ /*se il carattere letto corrisponde a quello cercato*/
                    occ++; /*incremento il contatore delle occorrenze*/
                }
            }
            if(i != 0){ /*se non è il primo figlio*/
                nr = read(piped[i][0], &S, sizeof(S)); /*figlio legge dalla pipe la struct inviata dal figlio di indice precedente (nel caso sia il primo dal padre)*/
                if(nr != sizeof(S)){
                    printf("Figlio %d ha letto numero di byte sbagliati %d\n", i, nr);
                    exit(-1);
                }
            } else {
                close(piped[0][0]); /*chiudo la pipe in lettura che era rimasta aperta siccome il primo figlio non la usa*/
            }
            
            /*figlio aggiorna i campi della struct*/ 
            S.c3+=occ;
            if(S.c1<occ){
                S.c1 = occ;
                S.c2 = i;
            }
            printf("DEBUG:FIGLIO %d STA PER SCRIVERE STRUCT CON CAMPI c1=%ld, c2=%d, c3=%ld\n", i, S.c1, S.c2, S.c3);
            nw = write(piped[(i+1)%N][1], &S, sizeof(S)); /*figlio scrive sulla pipe la struct aggiotrnata che verrà letta dal figlio successivo (in caso sia l'ultimo dal padre)*/
            if(nw != sizeof(S)){
                printf("Figlio %d ha scritto numero di byte sbagliati %d\n", i, nw);
                exit(-1);
            }
            exit(i); /*figlio ritorna al padre il proprio indice d'ordine*/
        }
    }

    /* processo padre */
    //chiudo tutte le pipe in lettura e scrittura tranne la prima (aperta in lettura)
    for(k = 0; k < N; k++){    
        close(piped[k][1]);
        if (k != 0) {
            close(piped[k][0]);
        }
    }

    nr = read(piped[0][0], &S, sizeof(S)); /*padre legge dalla prima pipe la struct inviata dall'ultimo figlio*/
    if(nr != sizeof(S)){
        printf("Padre ha letto numero di byte sbagliati %d\n", nr);
        exit(-1);
    }

    printf("Il figlio con pid %d e indice %d associato al file %s ha trovato %ld occorrenze del carattere %c, mentre il numero totale di occorrenze trovate nei vari file vale %ld\n", pid[S.c2], S.c2, argv[S.c2+1], S.c1, Cx, S.c3);

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