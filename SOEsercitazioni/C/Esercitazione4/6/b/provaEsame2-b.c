#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork e exec
#include <fcntl.h>	// Includo la libreria per la funzione open e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>

struct occorrenzeCarattere{
    long int occorrenze;
    char carattere;
};

int main(int argc, char** argv) {
    
    if (argc < 4) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 3*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    int N = argc - 2;
    for(int i = 2; i < argc; i++){
        if (strlen(argv[i]) != 1) {
            printf("Errore, la stringa %s non e' un singolo carattere\n", argv[i]);
            exit(4);
        }
    }
    long int count;
    struct occorrenzeCarattere *conta = malloc(N*sizeof(struct occorrenzeCarattere));
    if(conta==NULL){
        printf("Errore in malloc\n");
        exit(8);
    }
    int piped[2]; /* array dinamico di pipe descriptors per comunicazioni figli-padre */
    if(pipe(piped) < 0){
        printf("Errore in pipe\n");
        exit(7);
    }
    
    for(int i = 0; i < N; i++){
        int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
        
        if ((pidFiglio = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(3);
        }
        
        if (pidFiglio == 0)
        {	/* processo figlio */
        /* Chiusura delle pipe non usate nella comunicazione con il padre */ 
        close(piped[0]);
            count = 0;
            char c;
            
            //controllo se il file e' accedibile
            int fd = 0; /*variabile che conterra il file descriptor del file che stiamo per aprire */
            if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
                exit(-1);
            }
            while(read(fd, &c, 1) != 0){
                if(c == argv[i+2][0]){
                    count++;
                }
            }
            conta[i].occorrenze = count;
            conta[i].carattere = argv[i+2][0];
            write(piped[1], &conta[i], sizeof(struct occorrenzeCarattere));
            exit(conta[i].carattere);
        }
        /* processo padre */
    }
    close(piped[1]);
    for(int i = 0; i < N; i++){
        read(piped[0], &conta[i], sizeof(struct occorrenzeCarattere));
        printf("Trovate %ld occorrenze del carattere %c\n", conta[i].occorrenze, conta[i].carattere);
    }
    for(int i = 0; i < N; i++){
        int pidFiglio;
        int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
        int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(6);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Il figlio con pid %d ha ritornato %c\n", pidFiglio, (char)ritorno);
        }
        
    }
    exit(0);
}