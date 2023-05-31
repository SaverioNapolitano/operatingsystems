#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, exec, read, write
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {

    int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
    int fd; /*variabile che conterra il file descriptor del file che verrà aperto con la open */
    int i; /*contatore for fork*/
    int j; /*contatore read*/
    int k; /*contatore for pipe*/
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    struct {
        int c1;
        int c2;
        char c3[250];
    } c;
    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    ///apro argc-1 pipes
    pipe_t* piped=malloc(sizeof(pipe_t)*(argc-1));
    if(piped==NULL){
        printf("Errore in malloc\n");
        exit(4);
    }
    for(int i=0;i<argc-1;i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i",i);
            exit(2);
        }
    }

    for(i = 0; i < argc-1; i++){
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(3);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */

            //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura
            for(k = 0; k < argc-1; k++){
                close(piped[k][0]);
                if (k != i) {
                    close(piped[k][1]);
                }
            }
            int pipeFN[2]; /*creo la pipe di comunicazione fra figlio e nipote*/
            
            if (pipe(pipeFN) < 0 )
            {	printf("Errore creazione pipe\n");
                exit(-3);
            }
            //controllo se il file è accedibile
            fd = 0;
            if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(-1);
            }
            close(fd); /*chiudo il file siccome il figlio non lo usa*/
            
            if ((pidFiglio = fork()) < 0)	/* Il processo figlio crea un nipote, NB: riutilizzo variabile pidFiglio */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(-2);
            }
            
            if (pidFiglio == 0)
            {	/* processo nipote */
                close(1); /*chiudo standard output nipote*/
                dup(pipeFN[1]); /*collego lato scrittura della pipe allo standard output del nipote*/
                close(pipeFN[0]); close(pipeFN[1]); /*chiudo la pipe siccome non serve più*/
                execlp("sort", "sort", "-f", argv[i+1], (char *)0);
                /*torno qui solo in caso di errore*/
                printf("Errore in exec\n");
                exit(-1);
            }
            /* processo figlio */
            close(pipeFN[1]); /*chiudo lato scrittura pipe siccome figlio sarà lettore*/
            /*figlio aspetta nipote*/
            if ((pidFiglio = wait(&status)) < 0) {
                printf("Non e' stato creato nessun processo figlio\n");
                exit(-3);
            }
            
            if ((status & 0xFF) != 0)
            {
                printf("Il processo nipote è stato terminato in modo anomalo\n");
                exit(-4);
            } else {
                ritorno = (status >> 8) & 0xFF;
                j = 0; /*inizializzo contatore*/
                c.c1 = pidFiglio; /*salvo nel primo campo della struct il pid del nipote*/
                while(read(pipeFN[0], &c.c3[j], 1)){ /*finché ci sono caratteri da leggere*/
                    if(c.c3[j]=='\n'){
                        c.c3[++j]=0; /*metto terminatore di stringa*/
                        c.c2 = j; /*salvo nel secondo campo della struct la lunghezza della linea incluso terminatore*/
                        /*ho finito di leggere la prima linea*/
                        break; /*esco dal ciclo*/
                    }
                    j++; /*incremento contatore*/
                }
                /*figlio comunica la struct al padre scrivendola sulla pipe*/
                write(piped[i][1], &c, sizeof(c));
                exit(j-1); /*figlio ritorna al padre lunghezza linea senza terminatore*/
            }
        }
        /* processo padre */
    }

    //chiudo tutte le pipe in scrittura
    for(int k = 0; k < argc-1; k++){
        close(piped[k][1]);
    }

    for(i=0; i<argc-1;i++){
        read(piped[i][0], &c, sizeof(c));
        for(j=0;;j++){
            if(c.c3[j]=='\n'){
                c.c3[j]=0; /*metto il terminatore di stringa*/
                break; /*esco dal ciclo*/
            }
        }
        printf("Il nipote %d ha trovato una linea lunga %d che è: '%s' per il file %s\n", c.c1, c.c2, c.c3, argv[i+1]);
    }
    /*padre aspetta i figli*/
    for(i = 0; i<argc-1;i++){
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(3);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Figlio con pid %d ha ritornato %d\n", pidFiglio, ritorno);
        }
    }

    exit(0);
}