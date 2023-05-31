#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <ctype.h>
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int finitof(int *finito, int N){

    for(int i = 0; i < N; i++){
        if(!finito[i]){ /*se c'è almeno un figlio che non è finito*/
            return 0; 
        }
    }
    /*se tutti i figli sono finiti*/
    return 1;
}

int main(int argc, char** argv) {

    char max; /*carattere massimo ricevuto dal padre*/
    char ch; /*carattere letto dai file dai figli*/
    char ok; /*segnale inviato dal padre ai figli con l'indicazione se stampare o meno: 'y' = stampa, 'n' = non stampare*/
    int N; /*numero dei file*/
    int i; /*indice per i processi figli*/
    int j; /*indice per cicli*/
    int pid;	// memorizza il valore di ritorno della funzione fork
    int fd; // variabile che conterra' il file descriptor del file argv[i+1] che verrà aperto con la open
    int nr; /*variabile per il valore di ritorno della read*/
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    int *finito; /*array che memorizza lo stato dei figli: 0 se il figlio non è finito, 1 se il figlio è finito*/
    long int cur; /*posizione all'interno del file*/
    pipe_t* pipedFP; /*pipe per comunicazione figli-padre*/
    pipe_t* pipedPF; /*pipe per comunicazione padre-figli*/
    
    if (argc < 3) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    
    /*calcolo numero di file passati*/
    N = argc - 1;

    //allocazione memoria per gli array dinamici di N pipes ciascuno
    pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    pipedPF = (pipe_t*)malloc(sizeof(pipe_t) * (N));
    if (pipedFP == NULL || pipedPF == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(2);
    }
    /*allocazione memoria per array dinamico contenente lo stato dei figli*/
    finito = (int *)malloc(N*sizeof(int));
    if(finito == NULL){
        printf("Errore in malloc\n");
        exit(3);
    }

    /*all'inizio suppongo nessun figlio sia terminato: inizializzo perciò l'array riempiendolo di 0*/
    memset(finito, 0, N*sizeof(int));
    
    /*creazione delle N pipes per ciascuno dei due array*/
    for(i = 0; i < N; i++){
        if(pipe(pipedFP[i])<0){
            printf("Errore nella creazione della pipe figlio-padre numero:%d\n",i);
            exit(4);
        }

        if(pipe(pipedPF[i])<0){
            printf("Errore nella creazione della pipe padre-figlio numero:%d\n",i);
            exit(5);
        }
    }


    for(i = 0; i < N; i++){
        
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(6);
        }
        
        if (pid == 0)
        {	/* processo figlio */
            //figlio legge solo dalla pipe su cui scrive il padre di indice j e scrive solo sulla sua pipe di indice j
            for(j = 0; j < N; j++){
                close(pipedPF[j][1]);
                close(pipedFP[j][0]);
                if (j != i) {
                    close(pipedPF[j][0]);
                    close(pipedFP[j][1]);
                }
            }
            //apro il file argv[i+1]
            
            if((fd = open(argv[i+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[i+1], fd);
                exit(-1);
            }

            
            cur = 0L; /*inizializzo posizione all'interno del file*/
            while (read(fd, &ch, sizeof(char)))	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(islower(ch)){ /*se il carattere letto è alfabetico minuscolo*/
                    write(pipedFP[i][1], &ch, sizeof(char)); /*figlio comunica al padre il carattere letto*/
                    read(pipedPF[i][0], &ok, sizeof(char)); /*figlio legge l'indicazione del padre*/
                    if(ok == 'y'){
                        printf("il figlio di ordine %d e pid %d ha trovato il carattere %c in posizione %ld nel file %s\n", i, getpid(), ch, cur, argv[i+1]);
                    }
                }
                ++cur; /*incremento la posizione all'interno del file*/
            }
            
            exit(0);
        }
       
        
    }

    /* processo padre */
    //padre legge da tutte le pipe su cui scrive il figlio e scrive su tutte le sue pipe
    for(i = 0; i < N; i++){
        close(pipedFP[i][1]);
        close(pipedPF[i][0]);
    }

    
    j = 0; /*inizializzo l'indice del massimo*/
    while(!finitof(finito, N)){ /*finché almeno un figlio non è terminato*/
        max = 0; /*inizializzo il massimo*/
        for(i = 0; i < N; i++){ /*padre rispett l'ordine dei file*/
            if(!finito[i]){ /*se il figlio non è terminato*/
                nr = read(pipedFP[i][0], &ch, sizeof(char)); /*padre legge carattere inviato dal figlio*/
                if(nr != sizeof(char)){ /*se ho avuto problemi nella lettura*/
                    finito[i] = 1; /*aggiorno lo stato del figlio*/
                } else { /*altrimenti se ho letto correttamente*/
                    if(ch > max){ /*se è maggiore del carattere memorizzato in max*/
                        max = ch; /*aggiorno il massimo*/
                        j = i; /*aggiorno l'indice del massimo*/
                    }
                }  
            }
        }
        /*dopo che il padre ha letto da tutti i figli ancora in esecuzione il carattere inviato*/
        for(i = 0; i < N; i++){
            if(!finito[i]){ /*se il figlio è ancora in esecuzione*/
                if(i == j){ /*al figlio che ha trovato il massimo*/
                    ok = 'y'; 
                    write(pipedPF[i][1], &ok, sizeof(char)); /*il padre invierà l'indicazione di stampare*/
                } else {
                    ok = 'n';
                    write(pipedPF[i][1], &ok, sizeof(char)); /*il padre invierà l'indicazione di non stampare*/
                }
            }
            
        }
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