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
    char *chars; /*array per memorizzare i caratteri da cercare*/
    int Q; /*numero di processi figli*/
    int L; /*lunghezza in linee del file*/
    int q; /*indice processi figli*/
    int i; /*contatore*/
    int pid;	// memorizzo il valore di ritorno della funzione fork e della wait
    int fd; /*variabile che conterra il file descriptor del file che verrà aperto con la open */
    int occorrenze; /*contiene le occorrenze per ogni linea del carattere cercato*/
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    pipe_t* piped; /*pipe per la comunicazione a ring padre-figli*/
    
    if (argc < 5) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 4*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    Q = argc - 3;
    
    //controllo utlizzando la funzione atoi se il argv[2] e un numero strettamente positivo
    L = atoi(argv[2]);
    if(L <= 0){
        printf("il parametro %s non un numero positivo\n",argv[2]);
        exit(2);
    }

    chars = (char*)malloc(Q * sizeof(char));
    if(chars == NULL){
        printf("Errore nella malloc\n");
        exit(3);
    }

    for(q = 0; q < Q; q++){
        //controlla che la stringa argv[q+3] sia un singolo carattere
        if (strlen(argv[q+3]) != 1) {	
            printf("Errore, la stringa %s non e' un singolo carattere\n", argv[q+3]);
            exit(4);
        }
        chars[q] = argv[q+3][0]; /*isolo il carattere*/
    }

    //apro Q+1 pipes
    piped = (pipe_t*)malloc(sizeof(pipe_t) * (Q+1));
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(5);
    }
    
    for(i = 0;i < Q+1;i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(6);
        }
    }

    /*Ciclo di generazione dei figli*/
    for(q = 0; q < Q; q++){
        
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(5);
        }
        
        if (pid == 0)
        {	/* processo figlio */
            //chiudo tutte le pipe in lettura tranne quella di indice corrispondente al processo corrente
            //e tutte quelle in scrittura quella di indice corrispondente al processo corrente + 1
            for(i = 0; i < Q+1; i++){
                if(i != q+1){
                    close(piped[i][1]);
                }
                if (i != q) {
                    close(piped[i][0]);
                }
            }
            //controllo se il file e' accedibile
            fd = 0;
            if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
                exit(-1); /*ritorno -1 che sarà interpretato come 255 e quindi come errore dal momento che ho supposto che le occorrenze siano < 255*/
            }
            int last_occ = 0;
            occorrenze = 0; /*inizializzo contatore occorrenze*/
            while(read(fd, &c, sizeof(char))){ /*finché ci sono caratteri da leggere*/

                if(c == chars[q]){ /*se ho trovato il carattere che cerco*/
                    occorrenze++; /*incremento il contatore delle occorrenze*/
                }
                if(c == '\n'){ /*se trovo il terminatore di linea*/
                    if(read(piped[q][0], &c, sizeof(char)) != sizeof(char)){
                        printf("Figlio ha letto un numero di byte sbagliati\n");
                    } /*leggo dalla pipe corrispondente all'indice del processo per capire quando posso procedere*/
                    printf("Figlio con indice %d e pid %d ha letto %d caratteri %c nella linea corrente\n", q, getpid(), occorrenze, chars[q]);
                    if(write(piped[q+1][1], &c, sizeof(char))!=1){
                        printf("Figlio ha scritto un numero di byte sbagliati\n");
                    } /*invio al figlio successivo l'indicazione che può procedere*/
                    last_occ = occorrenze;
                    occorrenze = 0;
                }
            }
            /*al termine della lettura di tutto il file il figlio ritorna al padre il numero di occcorrenze trovate nell'ultima linea*/
            exit(last_occ);
        }
    }

    /* processo padre */
    //chiudo tutte le pipe in lettura tranne quella di indice 0 (la prima)
    //e tutte le pipe in scrittura tranne quella di indice Q (l'ultima)
    for(i = 0; i < Q+1; i++){
        if(i != Q)
        close(piped[i][0]);
        if (i != 0) {
            close(piped[i][1]);
        }
    }

    c = 'y';
    for(i = 0; i < L; i++){ /*per ogni linea del file*/
        printf("Linea %d del file %s\n", i+1, argv[1]);
        if(write(piped[0][1], &c, sizeof(char))!=1){
            printf("Padre ha scritto un numero di byte sbagliati\n");
        } /*padre comunica al primo figlio di procedere*/
        if(read(piped[Q][0], &c, sizeof(char))!=1){
            printf("Padre ha letto un numero di byte sbagliati\n");
        } /*padre attende che l'ultimo figlio gli comunichi che può procedere*/
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
            printf("Il figlio con pid %d ha ritornato %d\n", pid, ritorno);
        }
    }


    exit(0);
}