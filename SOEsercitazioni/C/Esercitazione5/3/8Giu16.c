#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, exec, read, write
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <time.h>
#define PERM 0640
//definisco il tipo pipe_t
typedef int pipe_t[2];

int mia_random(int n)
{
    int casuale;
    casuale = rand() % n;
    return casuale;
}

int main(int argc, char** argv) {
    int H;		/*Inizializzo varibile che conterra' argv[argc-1]*/
    int Fcreato; /*variabile che conterra il file descriptor del file che verrà creato con la creat */
    pipe_t* pipedFiglioScrittore; /*array di pipe per comunicazione padre-figli con figli che scrivono*/
    pipe_t* pipedPadreScrittore; /*array di pipe per comunicazione padre-figli con padre che scrive*/
    int i; /*contatore*/
    int j; /*contatore*/
    int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
    int fd; /*variabile che conterra il file descriptor del file che verrà aperto con la open */
    char s[255]; /*variabile che conterrà la linea letta dal file*/
    int lunghezza; /*lunghezza della linea scelta*/
    int indice; /*indice della linea*/
    int occorrenze; /*numero di volte in cui figlio ha scritto carattere su file*/
    int lunghezzaLinea; /*lunghezza della linea comunicata dal figlio al padre*/
    int random; /*numero casuale generato*/
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio

    if (argc < 6) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 4*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    //controllo utlizzando la funzione atoi se il argv[argc-1] e un numero strettamente positivo
    if((H = atoi(argv[argc-1])) <= 0 || H >= 255){
        printf("il parametro %s non un numero positivo o non strettamente minore di 255\n",argv[argc-1]);
        exit(2);
    }

    srand(time(NULL)); /*inizializzo seme*/

    
    //creo il file
    Fcreato = 0;
    if((Fcreato = creat("/tmp/creato", 0640)) < 0){		/* ERRORE se non si riesce a creare il file */
        printf("Errore in creazione del file /tmp/creato dato che fd = %d\n", Fcreato);
        exit(3);
    }

    //apro argc-2 pipes
    pipedFiglioScrittore=malloc(sizeof(pipe_t)*(argc-2));
    if(pipedFiglioScrittore==NULL){
        printf("Errore in malloc\n");
        exit(4);
    }

    for(i=0;i<argc-2;i++){
        if(pipe(pipedFiglioScrittore[i])<0){
            printf("Errore nella creazione della pipe numero:%i",i);
            exit(5);
        }
    }

    pipedPadreScrittore=malloc(sizeof(pipe_t)*(argc-2));
    if(pipedPadreScrittore==NULL){
        printf("Errore in malloc\n");
        exit(6);
    }

    for(i=0;i<argc-2;i++){
        if(pipe(pipedPadreScrittore[i])<0){
            printf("Errore nella creazione della pipe numero:%i",i);
            exit(7);
        }
    }

    for(i=0; i<argc-2; i++){
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(8);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
            printf("DEBUG:ENTRO NEL FIGLIO\n");
            occorrenze = 0;
            //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in lettura per le pipe in cui scrive il padre
            for(int k = 0; k < argc-2; k++){
                close(pipedPadreScrittore[k][1]);
                if (k != i) {
                    close(pipedPadreScrittore[k][0]);
                }
            }
            //chiudo tutte le pipe in lettura e scrittura tranne la i-esima pipe, aperta in scrittura per le pipe in cui scrive il figlio
            for(int k = 0; k < argc-2; k++){
                close(pipedFiglioScrittore[k][0]);
                if (k != i) {
                    close(pipedFiglioScrittore[k][1]);
                }
            }
            j = 0;
            fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(-1);
            }
            
            while (read(fd, &s[j], 1) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                printf("DEBUG:ENTRO NEL CICLO DI LETTURA NEL FIGLIO\n");
                if(s[j]=='\n'){ /*se ho trovato terminatore di lines*/
                    printf("DEBUG:FIGLIO HA TROVATO UNA LINEA\n");
                    j++;
                    write(pipedFiglioScrittore[i][1], &(j), sizeof(j)); /*figlio comunica al padre la lunghezza della linea letta*/
                    printf("DEBUG:FIGLIO HA SCRITTO LINEA\n");
                    read(pipedPadreScrittore[i][0], &indice, sizeof(indice)); /*figlio legge indice comunicato dal padre*/
                    printf("DEBUG:FIGLIO HA LETTO INDICE %i\n", indice);
                    if(indice<j){ /*se indice ammissibile*/
                        write(Fcreato, &s[indice], 1); /*scrivo su file il carattere*/
                        occorrenze++; /*incremento contatore*/
                        printf("DEBUG:FIGLIO HA SCRITTO UN CARATTERE %c\n", s[indice]);
                    }
                    j = -1; /*reinizializzo contatore per leggere nuova linea*/
                }
                j++;
            }

            exit(occorrenze);
        }
        /* processo padre */
        
    }

    //chiudo tutte le pipe dove scrive figlio in scrittura e dove scrive padre in lettura
    for(i = 0; i < argc-2; i++){
        close(pipedFiglioScrittore[i][1]);
        close(pipedPadreScrittore[i][0]);
    }
    printf("DEBUG:PADRE HA CHIUSO PIPE\n");
    for(i=0; i<H;i++){
        random=mia_random(argc-2); /*padre individua casualmente la lunghezza da considerare*/
        printf("DEBUG:PADRE HA GENERATO NUMERO CASUALE\n");
        for(j=0;j<argc-2;j++){
            read(pipedFiglioScrittore[j][0], &lunghezzaLinea, sizeof(lunghezzaLinea)); /*padre legge lunghezza inviata dal figlio*/
            printf("DEBUG:PADRE HA LETTO DALLA PIPE\n");
            if(j==random){
                lunghezza=lunghezzaLinea;
            }
        }
        indice=mia_random(lunghezza); /*padre individua casualmente l'indice del carattere da scrivere su file*/
        printf("DEBUG:PADRE HA GENERATO INDICE CASUALE\n");
        /*padre comunica a tutti i figli l'indice*/
        for(j = 0; j< argc-2;j++){
            write(pipedPadreScrittore[j][1], &indice, sizeof(indice));
            printf("DEBUG:PADRE HA SCRITTO SULLA PIPE\n");
        }
    }
    for(i=0;i<argc-2;i++){
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(9);
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