#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#define MSGSIZE 73
//definisco il tipo pipe_t
typedef int pipe_t[2];
int main(int argc, char** argv) {

    char linea[MSGSIZE]; /*variabile per memorizzare la linea letta dai figli dal file associato*/
    char buffer[MSGSIZE]; /*variabile per leggere il nome inviato dal primo figlio ai fratelli*/
    int N; /*numero di file*/
    int n; /*indice processi figli*/
    int j; /*indice*/
    int pid;	// memorizza il valore di ritorno della funzione fork
    int fd; /*variabile che conterra il file descriptor del file che verrà aperto con la open */
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    pipe_t* pipedFF; /*pipe per la comunicazione fra il primo figlio e i suoi fratelli*/
    
    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    /*calcoliamo il numero di file passati*/
    N = argc - 1;

    //allocazione di memoria per array dinamico di N - 1 pipes
    pipedFF = (pipe_t*)malloc(sizeof(pipe_t) * (N-1));
    if (pipedFF == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(2);
    }
    /*creazione di N - 1 pipes*/
    for(n = 0; n < N-1; n++){
        if(pipe(pipedFF[n])<0){
            printf("Errore nella creazione della pipe numero:%i\n", n);
            exit(3);
        }
    }

    for(n = 0; n < N; n++){
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(4);
        }
        
        if (pid == 0)
        {	/* processo figlio */
            //controllo se il file e' accedibile
            if((fd = open(argv[n+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[n+1], fd);
                exit(N); /*in caso di errore si tornano numeri crescenti maggiori di N - 1 che non sono valori ammissibili*/
            }

            if(n == 0){ /*se è il primo figlio*/
                //primo figlio non legge da nessuna pipe ma scrive su tutte
                for(j = 0; j < N-1; j++){
                    close(pipedFF[j][0]);
                }

                j = 0; /*inizializzo indice per ciclo lettura*/
            
                while (read(fd, linea, MSGSIZE))	/* ciclo di lettura fino a che riesco a leggere una linea da file */
                {        
                    linea[MSGSIZE-1]=0; /*trasformo la linea in stringa*/
                    for(j = 0; j < N - 1; j++){ /*e la comunico agli altri processi figli*/
                        write(pipedFF[j][1], linea, MSGSIZE);
                    }
                }
            } else { /*se non è il primo figlio*/

                //ogni figlio diverso dal primo non scrive su nessuna pipe e legge solo da quella corrispondente al proprio numero d'ordine - 1
                for(j = 0; j < N-1; j++){
                    close(pipedFF[j][1]);
                    if (j != n-1) {
                        close(pipedFF[j][0]);
                    }
                }

                while(read(pipedFF[n-1][0], buffer, MSGSIZE)){ /*per ogni nome letto dalla sua pipe con il primo figlio*/
                    buffer[MSGSIZE-1] = 0; /*zero-termino la stringa*/
                    lseek(fd, 0L, SEEK_SET); /*riporto il file pointer all'inizio del file*/
                    while(read(fd, linea, MSGSIZE)){ /*fino a quando riesco a leggere una linea dal file associato*/
                        linea[MSGSIZE-1] = 0; /*zero-termino la stringa*/
                        if ((pid = fork()) < 0)	/* Il processo figlio crea un nipote */
                        {	/* In questo caso la fork e' fallita */
                            printf("Errore durante la fork\n");
                            exit(N + 1);
                        }
                        
                        if (pid == 0)
                        {	/* processo nipote */
                            close(pipedFF[n-1][0]);
                            close(1); /*ridireziono standard output su /dev/null perché mi interessa solo se la diff ha successo*/
                            open("/dev/null", O_WRONLY);
                            execlp("diff", "diff", buffer, linea, (char *)0); /*nipote esegue comando diff per confrontare i due file*/
                            /* si esegue l'istruzione seguente SOLO in caso di fallimento */
                            
                            perror("errore esecuzione comando"); /*uso perror perché avendo ridiretto standard output non posso usare printf*/
                            exit(N + 2);
                        }
                        /*figlio aspetta il nipote*/
                        if ((pid = wait(&status)) < 0) {
                            printf("Non e' stato creato nessun processo nipote\n");
                            exit(N + 3);
                        }
                        
                        if ((status & 0xFF) != 0)
                        {
                            printf("Il processo nipote è stato terminato in modo anomalo\n");
                        } else {
                            ritorno = (status >> 8) & 0xFF;
                            if(ritorno == 0){ /*se il comando diff ha avuto successo (cioè i due file sono uguali)*/
                                printf("I file %s e %s sono uguali\n", buffer, linea);
                            }
                        }
                    }   
                }
                
            }
            /*al termine dell'esecuzione ogni figlio deve ritornare al padre il proprio numero d'ordine*/
            exit(n);
        }
    }

    /* processo padre */

    //padre non usa pipe quindi le chiude tutte in lettura e scrittura
    for(n = 0; n < N-1; n++){
        close(pipedFF[n][1]);
        close(pipedFF[n][0]);
    }

    /*padre aspetta i figli*/
    for(n = 0; n < N; n++){
        if ((pid = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(5);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Figlio con pid %d ha ritornato %d\n", pid, ritorno);
        }
    }
    
    exit(0);
}