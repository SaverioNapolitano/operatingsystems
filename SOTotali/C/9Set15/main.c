#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, exec, read, write
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <signal.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {

    int fd; /*variabile che conterra il file descriptor del file che stiamo per aprire */
    int i; /*contatore*/
    int k; /*contatore*/
    int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
    pipe_t *pipedPF; /*pipe che il padre userà per comunicare coi figli (padre scrittore)*/
    pipe_t* pipedFP; /*pipe che i figli useranno per comunicare col padre (figli scrittori)*/
    char c; /*carattere che verrà inviato dal padre al figlio*/
    char cf; /*carattere che il figlio leggerà dal file*/
    int pid[argc-2]; /*array in cui il padre salverà il pid dei figli*/
    int* confronto; /*array in cui è memorizzato il risultato del confronto di ogni figlio: 0 successo, 1 insuccesso*/
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio

    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    //apro argc-2 pipes
    pipedPF = (pipe_t*)malloc(sizeof(pipe_t) * (argc-2));
    if (pipedPF == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(2);
    }
    
    for(i = 0;i < argc-2;i++){
        if(pipe(pipedPF[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(3);
        }
    }

    //apro argc-2 pipes
    pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * (argc-2));
    if (pipedFP == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(4);
    }
    
    for(int i = 0;i < argc-2;i++){
        if(pipe(pipedFP[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(5);
        }
    }
    //controllo se il file e' accedibile
    fd = 0;
    if((fd = open(argv[argc-1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
        printf("Errore in apertura file %s dato che fd = %d\n", argv[argc], fd);
        exit(6);
    }

    confronto = (int *)calloc(argc-2, sizeof(int));

    for(i = 0; i < argc - 2; i++){
        
        if ((pid[i] = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(7);
        }
        
        if (pid[i] == 0)
        {	/* processo figlio */
            /*chiudo tutte le pipe in cui scrive il padre in scrittura e tutte quelle in cui scrivono i figli in lettura
            chiudo tutte le pipe in cui scrive il padre in lettura e tutte quelle in cui scrivono i figli in scrittura
            tranne quelle associate al figlio corrispondente*/
            for(k = 0; k < argc-2; k++){
                close(pipedPF[k][1]);
                close(pipedFP[k][0]);
                if (k != i) {
                    close(pipedPF[k][0]);
                    close(pipedFP[k][1]);
                }
            }
            //controllo se il file e' accedibile
            if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(-1);
            }
            printf("DEBUG:FIGLIO PRIMA DEL WHILE\n");
            while(read(pipedPF[i][0], &c, 1)){ 
                if(c=='t'){ /*se il padre comunica al figlio di terminare*/
                    break;
                }
                printf("DEBUG:FIGLIO ENTRA NEL WHILE\n");
                read(fd, &cf, 1); /*figlio legge carattere dal file*/
                printf("DEBUG:FIGLIO HA LETTO\n");
                write(pipedFP[i][1], &cf, 1); /*figlio comunica al padre il carattere che ha letto*/
                printf("DEBUG:FIGLIO HA SCRITTO\n");
            }
            /*se arrivo qui figlio termina normalmente*/
            printf("DEBUG:FIGLIO TERMINA NORMALMENTE\n");
            exit(0);
        }
        /* processo padre */
        
    }

    //chiudo tutte le pipe in cui scrive il padre in lettura e in cui scrivono i figli in scrittura
    for(k = 0; k < argc-2; k++){
        close(pipedPF[k][0]);
        close(pipedFP[k][1]);
    }

    while(read(fd, &cf, 1)){ /*finché il padre riesce a leggere dal file*/
        c = 'v';
        for(i=0; i<argc-2;i++){
            if(!confronto[i]){ /*se in posizione i dell'array confronto c'è zero allora padre manda segnale di procedere al figlio*/
                write(pipedPF[i][1], &c, 1);
            }
        }

        for(i = 0; i<argc-2;i++){
            if(!confronto[i]){/*se figlio ha confronto = 0 padre legge carattere scritto sulla pipe*/
                read(pipedFP[i][0], &c, 1); 
                if(c != cf){ /*se il carattere letto dal figlio è diverso da quello letto dal padre*/
                    confronto[i]=1; /*aggiorno valore di confronto*/
                }
            }
        }
    }

    /*quando padre ha finito di leggere il file*/
    c = 't';
    for(i=0; i<argc-2;i++){
        if(confronto[i]){ /*se figlio ha avuto insuccesso nel confronto*/
            kill(pid[i], SIGKILL); /*padre ne forza la terminazione*/
        } else {
            write(pipedPF[i][1], &c, 1); /*altrimenti dice al figlio di terminare normalmente*/
            printf("DEBUG:PADRE HA SCRITTO\n");
        }
    }
    /*padre aspetta i figli*/
    for(i=0; i<argc-2;i++){
        printf("DEBUG:ENTRO NEL FOR del figlio %i\n", i);
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(8);
        }
        printf("DEBUG:DOPO LA WAIT\n");
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio con pid %d è stato terminato in modo anomalo\n", pidFiglio);
        } else {
            printf("DEBUG:ENTRO NELL'ELSE\n");
            ritorno = (status >> 8) & 0xFF;
            for(k=0; k<argc-2; k++){
                if(pid[k]==pidFiglio){ /*quando individuo la posizione del figlio nell'array di pid esco dal ciclo*/
                    break; 
                }
            }
            printf("Il processo figlio con pid %d e file %s ha ritornato %d\n", pidFiglio, argv[i+1], ritorno);
        }
    }
    exit(0);
}