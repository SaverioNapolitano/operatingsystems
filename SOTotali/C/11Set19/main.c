#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
#define PERM 0644
//definisco il tipo pipe_t
typedef int pipe_t[2];
int main(int argc, char** argv) {
    
    char cd; /*carattere usato per leggere il contenuto del file dal figlio di indice dispari*/
    char cp;	// carattere usato per leggere il contenuto del file dal figlio di indice pari
    char *Fcreato; /*stringa che conterrà il nome del file creato dal processo di indice dispari*/
    int N; /*numero di file*/
    int L; /*lunghezza in caratteri dei file (è la stessa per tutti)*/
    int i; /*indice per cicli*/
    int j; /*indice per cicli*/
    int pid;	// memorizza il valore di ritorno della funzione fork
    int fd; // variabile che conterra il file descriptor del file associato che ogni figlio aprirà con la open */
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    int fcreato; /*file descriptor del file creato dal processo di indice dispari*/
    int nr; /*per valore di ritorno della read*/
    int nw; /*per valore di ritorno della write*/
    pipe_t* piped; /*pipe di comunicazione fra processi di indice pari e processi di indice dispari*/
    if (argc < 3) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    /*calcoliamo il numero di file passati*/
    N = argc - 1;

    if(N%2 != 0){ /*se il numero non è pari*/
        printf("Il numero di file passati deve essere pari\n");
        exit(2);
    }

    //allocazione della memoria per l'array dinamico di N/2 pipes
    piped = (pipe_t*)malloc(sizeof(pipe_t) * (N/2));
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(3);
    }
    /*creazione delle N/2 pipes*/
    for(i = 0; i < N/2; i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%d\n",i);
            exit(4);
        }
    }

    //apro il file argv[1]
    if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
        printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
        exit(5);
    }

    /*ne calcolo la lunghezza*/
    L = lseek(fd, 0L, SEEK_END);

    close(fd); /*chiudo il file siccome il padre non lo usa*/

    /*ciclo di generazione degli N figli*/
    for(i = 0; i < N; i++){
        
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(6);
        }
        
        if (pid == 0)
        {	/* processo figlio */
            /*ogni figlio a prescindere dall'indice pari o dispari deve aprire il file in posizione i + 1*/
            //apro il file argv[i+1]
                
            if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(-1); /*in caso di errore si ritorna -1 che verrà interpretato dal padre come 255 che non è un valore ammissibile*/
            }

            if(i%2 == 0){ /*processo di indice pari*/
                printf("DEBUG:FIGLIO DI INDICE PARI %d associato al file %s\n", i, argv[i+1]);
                //il figlio di indice pari non legge da nessuna pipe e scrive solo su quella di indice i / 2
                for(j = 0; j < N/2; j++){
                    close(piped[j][0]);
                    if (j != i/2) {
                        close(piped[j][1]);
                    }
                }
                
                for(j = 0; j < L; j++){ /*leggo tutti i caratteri del file*/
                    nr = read(fd, &cp, sizeof(char)); 
                    if(nr != sizeof(char)){
                        printf("DEBUG:FIGLIO DI INDICE PARI %d HA LETTO NUMERO DI BYTE SBAGLIATI %d\n", i, nr);
                    }
                    printf("DEBUG:CARATTERE LETTO %c\n", cp);
                    nw = write(piped[i/2][1], &cp, sizeof(char)); /*comunica il carattere letto al figlio di indice dispari*/
                    printf("DEBUG:CARATTERE SCRITTO %c\n", cp);
                    if(nw != sizeof(char)){
                        printf("DEBUG:FIGLIO DI INDICE PARI %d HA SCRITTO NUMERO DI BYTE SBAGLIATI %d\n", i, nr);
                    }
                }
            } else { /*processo di indice dispari*/
                 printf("DEBUG:FIGLIO DI INDICE DISPARI %d associato al file %s\n", i, argv[i+1]);
                //figlio di indice dispari non scrive su nessuna pipe e legge solo da quella di indice i/2
                for(j = 0; j < N/2; j++){
                    close(piped[j][1]);
                    if (j != i/2) {
                        close(piped[j][0]);
                    }
                }
                Fcreato = (char *)malloc(strlen(argv[i+1])+10); /*alloco la memoria necessaria a contenere il nome del file che verrà creato, 
                dato dalla concatenazione del nome del file associato (la cui lunghezza senza terminatore è strlen(argv[i+1])), il punto '.' (1),
                la stringa "MAGGIORE" (8) e il terminatore di stringa (1) = strlen(argv[i+1]) + 1 + 8 + 1 = strlen(argv[i+1]) + 10*/
                strcpy(Fcreato, argv[i+1]); /*copio nella stringa allocata il nome del file associato*/
                strcat(Fcreato, ".MAGGIORE"); /*e lo concateno con la stringa ".MAGGIORE"*/
                //creo il file Fcreato
                
                if((fcreato = creat(Fcreato, PERM)) < 0){		/* ERRORE se non si riesce a creare il file */
                    printf("Errore in creazione del file %s dato che fcreato = %d\n", Fcreato, fcreato);
                    exit(-1);
                }

                for(j = 0; j < L; ++j){ /*leggo tutti i caratteri del file*/
                    nr = read(fd, &cd, sizeof(char)); /*prima il processo di indice dispari legge il proprrio carattere*/
                    if(nr != sizeof(char)){
                        printf("DEBUG:FIGLIO DI INDICE DISPARI %d HA LETTO NUMERO DI BYTE SBAGLIATI DA FILE %d\n", i, nr);
                    }
                    nr = read(piped[i/2][0], &cp, sizeof(char)); /*dopo legge carattere scritto sulla pipe da figlio di indice pari*/
                    if(nr != sizeof(char)){
                        printf("DEBUG:FIGLIO DI INDICE DISPARI %d HA LETTO NUMERO DI BYTE SBAGLIATI DA PIPE%d\n", i, nr);
                    }
                    printf("DEBUG:Confronto carattere letto da figlio pari %c con carattere letto da figlio dispari %c\n", cp, cd);
                    if(cd > cp){ /*se il carattere letto dal processo di indice dispari è strettamente maggiore di quello letto dal processo indice pari*/
                       nw = write(fcreato, &cd, sizeof(char)); /*lo scrive sul file creato*/
                       if(nw != sizeof(char)){
                            printf("DEBUG:FIGLIO DI INDICE DISPARI %d HA SCRITTO NUMERO DI BYTE SBAGLIATI %d\n", i, nw);
                        }
                    } else {
                        nw = write(fcreato, &cp, sizeof(char)); /*altrimenti scrive cp*/
                        if(nw != sizeof(char)){
                            printf("DEBUG:FIGLIO DI INDICE DISPARI %d HA SCRITTO NUMERO DI BYTE SBAGLIATI %d\n", i, nw);
                        }
                    }
                }

            }
            /*al termine dell'esecuzione entrambi i figli ritornano al padre il numero di caratteri letti dal proprio file*/
            exit(L);
        }
    }

    /* processo padre */
    //padre chiude tutte le pipe siccome non le usa
    for(i = 0; i < N/2; i++){
        close(piped[i][1]);    
        close(piped[i][0]);
    }

    /*padre aspetta i figli*/
    for(i = 0; i < N; i++){
        
        
        if ((pid = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(7);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Il figlio con pid %d ha ritornato %d (se 255 problemi)\n", pid, ritorno);
        }
    }


    exit(0);
}