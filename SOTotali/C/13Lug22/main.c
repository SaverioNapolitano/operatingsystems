#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];

int main(int argc, char** argv) {
    
    char ok; /*variabile usata per la sincronizzazione fra padre e figli (non importa che valore abbia)*/
    char ch;	// carattere usato per leggere il contenuto del file
    char C; /*carattere associato al file*/
    int L; /*lunghezza in linee del file F*/
    int F; // variabile che conterra' il file descriptor del file argv[1]
    int Q; /*numero di processi figli*/
    int q; /*indice dei processi figli*/
    int j; /*indice per cicli*/
    int ritorno; /*valore di ritorno dei figli al padre (occorrenze del carattere trovate nell'ultima linea)*/
    int occ; /*occorrenze del carattere C nel file argv[1]*/
    int pid;	// memorizza il valore di ritorno della funzione fork
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    pipe_t* piped; /*pipe per la comunicazione a ring fra padre e figli: si legge dalla pipe q e si scrive sulla pipe q + 1*/

    if (argc < 5) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 4*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    /*calcoliamo il numero di singoli caratteri passati*/
    Q = argc - 3;

    //controllo utlizzando la funzione atoi se il argv[2] e un numero strettamente positivo
    if((L = atoi(argv[2])) <= 0){
        printf("il parametro %s non un numero positivo\n",argv[2]);
        exit(2);
    }

    for(q = 3; q < argc; q++){
        //controlla che la stringa argv[q contenga un singolo carattere
        if (strlen(argv[q]) != 1) {
            printf("Errore, la stringa %s non e' un singolo carattere\n", argv[q]);
            exit(3);
        }
    }

    //allocazione della memoria per l'array dinamico di Q pipes
    piped = (pipe_t*)malloc(sizeof(pipe_t) * (Q + 1));
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(4);
    }
    /*creazione delle Q + 1 pipes*/
    for(q = 0; q < Q + 1; q++){
        if(pipe(piped[q])<0){
            printf("Errore nella creazione della pipe numero:%d\n",q);
            exit(5);
        }
    }
    /*ciclo di creazione dei Q figli*/
    for(q = 0; q < Q; ++q){
        
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(6);
        }
        
        if (pid == 0)
        {	/* processo figlio */
            //figlio scrive solo sulla pipe q+1 e legge solo da quella di indice q
            for(j = 0; j < Q + 1; j++){
                if(j != q+1){
                    close(piped[j][1]);
                }

                if (j != q) {
                    close(piped[j][0]);
                }
            }
            C = argv[q+3][0]; /*isolo il carattere per maggiore comodità*/
            //apro il file argv[1]
            
            if((F = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[1], F);
                exit(-1); /*in caso di errore si ritorna -1 che verrà interpretato come 255 che non è un valore ammissibile*/
            }

            
            occ = 0; /*inizializzo il contatore delle occorrenze*/
            ritorno = -1; /*inizializzo il valore di ritorno a -1 che nel caso non cambiasse verrebbe interpretato come 255 che non è un valore ammissibile*/
            while (read(F, &ch, sizeof(char)))	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(ch == '\n'){ /*se ho trovato il terminatore di linea*/
                    read(piped[q][0], &ok, sizeof(char)); /*figlio legge dalla pipe il carattere di sincronizzazione inviato dal figlio precedente (o dal padre nel caso di primo figlio)*/
                    printf("Figlio con indice %d e pid %d ha letto %d caratteri %c nella linea corrente\n", q, getpid(), occ, C);
                    write(piped[q+1][1], &ok, sizeof(char)); /*figlio comunica al figlio successivo (o al padre se si tratta dell'ultimo figlio) che può procedere*/
                    ritorno = occ; /*memorizza il numero di occorrenze*/
                    occ = 0; /*inizializza il contatore delle occorrenze per la nuova linea*/
                }

                if(ch == C){ /*se il carattere letto corrisponde a quello cercato*/
                    occ++; /*incremento il contatore delle occorrenze*/
                }
            }


            /*al termine dell'esecuzione figlio ritorna al padre il numero di occorrenze del carattere nell'ultima linea del file*/
            exit(ritorno);
        }
        
        
    }

    /* processo padre */
    //padre legge solo dall'ultima pipe e scrive solo sulla prima
    for(j = 0; j < Q + 1; j++){
        if(j != 0){
            close(piped[j][1]);
        }

        if(j != Q){
            close(piped[j][0]);
        }
        
    }

    for(j = 0; j < L; j++){ /*per ogni linea del file F*/
        printf("Linea %d del file %s\n", j+1, argv[1]);
        write(piped[0][1], &ok, sizeof(char)); /*padre comunica al primo figlio che può procedere con la stampa*/
        read(piped[Q][0], &ok, sizeof(char)); /*padre attende che l'ultimo figlio gli comunichi che può procedere*/
    }

    /*padre aspetta figli*/
    for(q = 0; q < Q; q++){
        
        if ((pid = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(7);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Figlio con pid %d ha ritornato %d (se 255 problemi)\n", pid, ritorno);
        }
    }
    exit(0);
}