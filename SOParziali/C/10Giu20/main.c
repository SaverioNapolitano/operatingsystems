#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <ctype.h>
#define PERM 0644
//definisco il tipo pipe_t
typedef int pipe_t[2];
int main(int argc, char** argv) {
    
    char linea[250]; /*variabile usata per memorizzare la linea letta (dal file per i figli e dalla pipe per il padre)*/
    int L; /*numero di linee inviate al padre dal figlio*/
    int Q; /*numero di parametri passati che corrisponde al numero di figli che dovranno essere creati*/
    int fd;/*variabile che conterra il file descriptor del file che verrà creato con la creat*/
    int q; /*indice*/
    int j; /*indice*/
    int pid;	// memorizzo il valore di ritorno della funzione fork
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    pipe_t* piped; /*pipe per comunicazione figli-padre*/

    if (argc < 3) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    /*calcoliamo il numero di processi figli da generare con la fork*/
    Q = argc - 1;

    //creo il file
    if((fd = creat("Camilla", PERM)) < 0){		/* ERRORE se non si riesce a creare il file */
        printf("Errore in creazione del file %s dato che fd = %d\n", "pwd/Camilla", fd);
        exit(2);
    }

    //allocazione della memoria per l'array dinamica di Q pipes
    piped = (pipe_t*)malloc(sizeof(pipe_t) * (Q));
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(3);
    }
    /*creazione delle Q pipes*/
    for(q = 0;q < Q;q++){
        if(pipe(piped[q])<0){
            printf("Errore nella creazione della pipe numero:%i\n",q);
            exit(4);
        }
    }

    /*ciclo di generazione dei Q processi figli*/
    for(q = 0; q < Q; q++){
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(3);
        }
        
        if (pid == 0)
        {	/* processo figlio q-esimo */
            //figlio non legge da nessuna pipe e scrive solo su quella di indice q
            for(j = 0; j < Q; j++){
                close(piped[j][0]);
                if (j != q) {
                    close(piped[j][1]);
                }
            }
            //controllo se il file e' accedibile
            if((fd = open(argv[q+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[q+1], fd);
                exit(-1); /*si ritorna in caso di errore -1 che verrà interpretato come 255 che è un valore non accettabile dal padre*/
            }

            L = 0; /*inizializzo numero di linee inviate dal figlio al padre*/
            j = 0; /*inizializzo indice*/
            while (read(fd, &linea[j], sizeof(char)))	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(linea[j]=='\n'){ /*se ho trovato terminatore di linea*/

                    if(isdigit(linea[0]) && (++j) < 10){ /*se il primo carattere di tale linea è numerico e la lunghezza (compreso il terminatore di linea) è strettamente minore di 10*/
                        write(piped[q][1], linea, j*sizeof(char)); /*figlio comunica la linea al padre*/
                        L++; /*incremento numero di linee inviate dal figlio al padre*/
                    }
                    j = 0; /*inizializzo il contatore per leggere la nuova linea*/

                } else {
                    j++; /*incremento il contatore*/
                }
            }
            
            /*alla fine del ciclo di lettura il figlio ritorna al padre il numero di linee che gli ha inviato (supposto strettamente minore di 255)*/
            exit(L);
        }
    }

    /* processo padre */
    //padre legge da tutte le pipe e non scrive su nessuna
    for(q = 0; q < Q; q++){
        close(piped[q][1]);
    }

    for(q = 0; q < Q; ++q){ /*padre riceve, rispettando l'ordine dei file, tutte le linee inviate da ogni singolo figlio*/
        j = 0;
        while (read(piped[q][0], &linea[j], sizeof(char)))	/* ciclo di lettura fino a che riesco a leggere un carattere dalla pipe */
        {
            if(linea[j]=='\n'){ /*se ho trovato terminatore di linea*/
                linea[j]=0; /*la trasformo in una stringa*/
                printf("Figlio di indice %d associato al file %s ha trovato linea '%s'\n", q, argv[q+1], linea);
                j=0; /*inizializzo indice per leggere prossima linea*/
            } else {
                j++; /*incremento contatore*/
            } 
        }
    }

    /*padre aspetta i figli*/
    for(q = 0; q < Q; q++){
        if ((pid = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(6);
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