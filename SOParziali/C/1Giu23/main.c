#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <string.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];
typedef struct {
    int c1; /*pid del nipote*/
    char c2[250]; /*ultima linea che il figlio ha ricevuto dal nipote*/
    int c3; /*lunghezza della linea (compreso il terminatore di linea)*/
} ST;

int main(int argc, char** argv) {

    /*------------------------------------------------*/
    char Buff[250];	/*array di caratteri usato dal figlio per leggere le linee inviate dal nipote*/
    int j; /*indice per cicli*/
    int Z; /*numero di file passati come argomenti (che corrisponde anche al numero di figli che verranno creati)*/
    int z; /*indice dei processi figli che verranno generati dal padre con una fork*/
    int pid;	/*memorizza il valore di ritorno della funzione fork e della wait*/
    int fd; /*variabile che conterra' il file descriptor del file associato ad ogni figlio/nipote*/
    int status;	/*La variabile usata per memorizzare quanto ritornato dalla primitiva wait*/
    int ritorno; /*La variabile usata per memorizzare il valore di ritorno del processo figlio (che sarà la lunghezza della stringa senza il terminatore di linea)*/
    int nr; /*per valore di ritorno della read*/
    int nw; /*per valore di ritorno della write*/
    pipe_t* pipedFP; /*pipes per la comunicazione fra figli e padre: padre leggerà da tutte le pipe e non scriverà su nessuna, mentre ogni 
    figlio scriverà sulla pipe di indice corrispondente al suo numero d'ordine e non leggerà da nessuna pipe*/
    ST strutt; /*struttura dati inviata dai figli e ricevuta dal padre: il primo campo contiene il pid del nipote generato dal figlio 
    corrispondente, il secondo campo contiene l'ultima linea che il figlio ha letto dal nipote, il terzo campo contiene la lunghezza della
    suddetta linea compreso il terminatore di linea*/
    /*------------------------------------------------*/

    if (argc < 3) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    /*calcolo il numero di file passati*/
    Z = argc - 1;

    /*allocazione della memoria per l'array dinamico di Z pipes fra i figli ed il padre*/
    pipedFP = (pipe_t*)malloc(sizeof(pipe_t) * Z);
    if (pipedFP == NULL) { /*se la malloc non ha avuto successo*/
        printf("Errore nell'allocazione della memoria\n");
        exit(2);
    }

    /*creazione delle Z pipes*/
    for(z = 0; z < Z; z++){
        if(pipe(pipedFP[z])<0){ /*se non si riesce a creare la pipe*/
            printf("Errore nella creazione della pipe fra figlio e padre numero:%d\n",z);
            exit(3);
        }
    }

    /*ciclo di generazione degli Z figli*/
    for(z = 0; z < Z; z++){
        
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(4);
        }
        
        if (pid == 0)
        {	/* processo figlio */
            /*il figlio non legge da nessuna pipe (e perciò le chiude tutte in lettura) e scrive solo sulla pipe di indice z (che è 
            l'unica che lascerà pertanto aperta in scrittura)*/
            for(j = 0; j < Z; j++){
                close(pipedFP[j][0]);
                if (j != z) {
                    close(pipedFP[j][1]);
                }
            }
            pipe_t pipedNF; /*singola pipe di comunicazione fra nipote e figlio, dal momento che il padre non ne fa uso conviene che sia il 
            figlio a crearla in modo da non complicare la chiusura delle pipe nel padre, nel nipote e nel figlio*/
            if(pipe(pipedNF) < 0){ /*se ci sono problemi nella creazione della pipe*/
                printf("Errore nella creazione della pipe fra nipote e figlio\n");
                exit(-1);
            }
            
            if ((pid = fork()) < 0)	/* Il processo figlio crea un nipote */
            {	/* In questo caso la fork e' fallita */
                printf("Errore durante la fork\n");
                exit(250); /*in caso di errore si decide di ritornare un numero crescente a partire da 250, che non è un valore ammissibile
                dal padre poiché si è supposto che la linea abbia lunghezza massima di 250 caratteri inclusi i terminatori di linea e di 
                stringa, e quindi di 249 caratteri compreso il solo terminatore di linea (che è ciò che il figlio deve ritornare al padre)*/
            }
            
            if (pid == 0)
            {	/* processo nipote */
                
                close(pipedFP[z][1]); /*nipote chiude la pipe lasciata aperta dal figlio poiché non la usa*/
                close(1); /*chiudo lo standard output del nipote in modo da poterlo ridirezionare sulla pipe: in questo modo il figlio scriverà
                su ciò che per lui è standard input ma il figlio potrà leggerne il contenuto*/
                /*duplico la pipe fra figlio e nipote lato  scrittura in modo che il nuovo file descriptor che viene creato vada a prendere il posto dello standard output nella tabella dei file aperti*/
                dup(pipedNF[1]);
                /*chiudo entrambi i lati della pipe fra figlio e nipote: il lato di lettura non viene utilizzato e quello di scrittura viene usato tramite la ridirezione*/
                close(pipedNF[1]);
                close(pipedNF[0]);

                /*apro il file associato al nipote*/
                if((fd = open(argv[z+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire il file in lettura */
                    printf("Errore in apertura file %s dato che fd = %d\n", argv[z+1], fd);
                    exit(-1);
                }

                /*nipote esegue comando unix rev sul file associato*/
                execlp("rev", "rev", argv[z+1], (char *)0);
                /* si esegue l'istruzione seguente SOLO in caso di fallimento */
                
                perror("errore esecuzione comando rev\n"); /* avendo ridiretto lo standard output sulla pipe non si può fare una printf, in caso di errori bisogna scrivere sullo standard error*/
                exit(-2);
            }
            /* processo figlio */
            close(pipedNF[1]); /*il figlio chiude il lato di scrittura della pipe col nipote siccome deve solo leggere*/
            strutt.c1 = pid; /*figlio inserisce nel campo c1 della struct il pid del nipote*/
            j = 0; /*inizializzo l'indice della stringa*/
            while (read(pipedNF[0], &Buff[j], sizeof(char)))	/* ciclo di lettura fino a che riesco a leggere un carattere dalla pipe */
            {
                if(Buff[j] == '\n'){ /*se ho trovato il terminatore di linea*/
                    ritorno = ++j; /*memorizzo la lunghezza della stringa incrementata di 1 poiché deve comprendere il terminatore di stringa*/
                    j = 0; /*azzero l'indice per prepararmi alla lettura della nuova linea*/
                } else { /*se non ho trovato il terminatore di linea*/
                    j++; /*incremento l'indice*/
                }
            }
            /*al termine del ciclo di lettura il figlio inizializza i campi della struttura dati da comunicare al padre*/
            for(j = 0; j < ritorno; j++){
                strutt.c2[j] = Buff[j]; /*figlio copia nel campo c2 della struct l'ultima linea letta che è memorizzata in Buff*/
            }
            strutt.c3 = ritorno; /*figlio memorizza nel campo c3 della struct la lunghezza (compreso il terminatore di linea) dell'ultima linea letta*/
            
            nw = write(pipedFP[z][1], &strutt, sizeof(strutt)); /*figlio comunica al padre la struttura dati*/
            if(nw != sizeof(strutt)){ /*se ho avyto problemi nella lettura*/
                printf("Errore: il figlio di indice %d e pid %d ha scritto sulla pipe un numero di byte sbagliati %i\n", z, getpid(), nw);
                exit(251);
            }
            /*figlio aspetta il nipote disinteressandosi del valore di ritorno*/
            if ((pid = wait(&status)) < 0) {
                printf("Non e' stato creato nessun processo figlio\n");
                exit(252);
            }
            /*figlio controlla solo se il nipote è terminato in modo anomalo così da poterlo eventualmente segnalare*/
            if ((status & 0xFF) != 0)
            {
                printf("Il processo nipote con PID %d è stato terminato in modo anomalo\n", pid);
            }

            /*al termine dell'esecuzione il figlio ritorna al padre la lunghezza della linea inviata al padre escluso il terminatore di stringa*/
            exit(--ritorno);
        }
    }

    /* processo padre */
    /*il padre non scrive su nessuna pipe (quindi le chiude tutte lato scrittura) ma legge da tutte (quindi le lascia tutte aperte lato lettura)*/
    for(z = 0; z < Z; z++){
        close(pipedFP[z][1]);
    }

    for(z = 0; z < Z; z++){ /*padre rispetta l'ordine dei file*/
        nr = read(pipedFP[z][0], &strutt, sizeof(strutt)); /*padre legge la struttura dati comunicata dal figlio*/
        if(nr != sizeof(strutt)){ /*se ho avuto problemi in lettura*/
            printf("Il padre ha letto dalla pipe di indice %d un numero di byte sbagliati %i\n", z, nr);
            exit(5);
        }
        strutt.c2[strutt.c3 + 1] = 0; /*padre trasforma in una stringa quanto ricevuto nel campo c2 della struct mantenendo però il fatto che sia una linea*/
        printf("Il nipote con pid %d associato al file %s ha mandato come ultima linea al figlio\n%sla cui lunghezza compreso il terminatore di linea è %d\n", strutt.c1, argv[z+1], strutt.c2, strutt.c3);
    }

    /*padre aspetta i figli*/
    for(z = 0; z < Z; z++){
        if ((pid = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(6);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio con PID %d è stato terminato in modo anomalo\n", pid);
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Il figlio con pid %d ha ritornato %d (se maggiore di 249 probemi)\n", pid, ritorno);
        }
    }

    exit(0);
}