#include <stdio.h>		// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>		// Includo la libreria per la funzione exit
#include <unistd.h>		// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>		// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <errno.h>
#define PERM 0644
//definisco il tipo pipe_t
typedef int pipe_t[2];
int main(int argc, char** argv) {

    char linea[200]; /*variabile per memorizzare la linea*/
    int N; /*numero di file/processi figli*/
    int L; /*lunghezza in linee di ogni file*/
    int l; /*lunghezza della singola linea letta (per il padre) e contatore del numero di linee (per il figlio)*/
    int n; /*indice dei processi figli*/
    int i; /*indice per il ciclo di lettura*/
    int fd; /*variabile che conterra il file descriptor del file che verrà creato con la creat e dei file che verranno aperti con la open */
    int pid;	// memorizza il valore di ritorno della funzione fork
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    int nr; /*per valore di ritorno read*/
    int nw; /*per valore di ritorno write*/
    pipe_t pipedFP; /*pipe per la comunicazione fra il padre e il figlio 'speciale'*/
    pipe_t* piped; /*pipe per la comunicazione fra i figli 'normali' e il padre*/

    if (argc < 3) /* controllo sul numero di parametri: devono essere in numero maggiore o uguale a 2*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    /*calcolo il numero di file/processi figli*/
    N = argc - 1;

    //creo il file
    if((fd = creat("/tmp/saverionapolitano", PERM)) < 0){		/* ERRORE se non si riesce a creare il file */
        printf("Errore in creazione del file %s dato che fd = %d\n", "saverionapolitano", fd);
        exit(2);
    }
    
    if (pipe(pipedFP) < 0 )
    {	printf("Errore creazione pipe\n");
        exit(3);
    }

    if ((pid = fork()) < 0)	/* Il processo padre crea il figlio 'speciale' */
    {	/* In questo caso la fork e' fallita */
        printf("Errore durante la fork\n");
        exit(4);
    }
    
    if (pid == 0)
    {	/* processo figlio 'speciale' */
        close(0); /*chiudo lo standard input in modo da poterlo ridirezionare sul file*/
        //controllo se il file e' accedibile
        if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
            printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
            exit(5);
        }
        close(1); /*chiudo lo standard output in modo da poterlo ridirezionare sulla pipe*/
        dup(pipedFP[1]);
        /*a questo punto chiudo entrambi i lati della pipe*/
        close(pipedFP[0]);
        close(pipedFP[1]);
        execlp("wc", "wc", "-l", (char *)0); /*il processo conta la lunghezza in linee del primo file passato (che sarà poi la lunghezza in linee di tutti i file)*/
        /* si esegue l'istruzione seguente SOLO in caso di fallimento */
        
        perror("errore esecuzione comando"); // 
        exit(errno);
    }
    /* processo padre */

    /*padre aspetta il figlio 'speciale'*/
   if ((pid = wait((int*) 0)) < 0) {
    printf("Non e' stato creato nessun processo figlio\n");
    exit(5);
   }

    close(pipedFP[1]); /*padre non scrive sulla pipe con il figlio 'speciale' ma legge */

    i = 0; /*inizializzo indice per il ciclo di lettura*/
    while (read(pipedFP[0], &linea[i], sizeof(char)))	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
    {
        i++;
    }

    /*quando ho finito di leggere dalla pipe devo inserire il terminatore di stringa*/
    linea[i]=0;
    /*a questo punto converto la stringa nell'intero che rappresenterà la lunghezza in linee dei file*/
    L = atoi(linea);
    if(L <= 0){
        printf("Errore nel calcolo della lunghezza dei file\n");
        exit(6);
    }

    close(pipedFP[0]);

    //allocazione memoria per array dinamico di N pipes
    piped = (pipe_t*)malloc(sizeof(pipe_t) * N);
    if (piped == NULL) {
        printf("Errore nell'allocazione della memoria\n");
        exit(7);
    }
    /*creazione di N pipes*/
    for(i = 0; i < N; i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(8);
        }
    }

    /*ciclo di generazione degli N processi figli*/
    for(n = 0; n < N; n++){
        
        if ((pid = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(9);
        }
        
        if (pid == 0)
        {	/* processo figlio */
            //chiudo tutte le pipe in lettura e scrittura tranne la n-esima pipe, aperta in scrittura (figlio non legge da nessuna pipe e scrive solo su quella di indice n)
            for(i = 0; i < N; i++){
                close(piped[i][0]);
                if (i != n) {
                    close(piped[i][1]);
                }
            }
            //controllo se il file e' accedibile
            if((fd = open(argv[n+1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[n+1], fd);
                exit(201); /*in caso di errore si ritorna un numero crescente sempre maggiore di 200 che non è ammissibile*/
            }
            
            i = 0; /*inizializzp indice per il ciclo di lettura*/
            l = 0; /*inizializzo numero di linee lette*/
            while (read(fd, &linea[i], sizeof(char)))	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(linea[i]=='\n'){ /*se ho trovato il terminatore di linea*/
                    i++; /*incremento la lunghezza della linea poiché deve comprendere anche il terminatore*/
                    nw = write(piped[n][1], &i, sizeof(i)); /*e la comunico al padre*/
                    if(nw != sizeof(i)){
                        printf("DEBUG:FIGLIO %d ha scritto numero di byte sbagliati %i nel comunicare la lunghezza\n", n, nw);
                    }
                    nw = write(piped[n][1], linea, i*sizeof(char)); /*a questo punto figlio comunica al padre la linea vera e propria*/
                    if(nw != i*sizeof(char)){
                        printf("DEBUG:FIGLIO %d ha scritto numero di byte sbagliati %i nel comunicare la linea\n", n, nw);
                    }
                    ++l; /*incremento numero di linee lette*/
                    if(l != L){ /*se la linea che ho letto non è stata l'ultima*/
                       i = 0; /*inizializzo contatore per leggere la prossima linea*/
                    }
                } else {
                    i++; /*se non ho trovato il terminatore di linea incremento il contatore*/
                }
            }

            /*al termine del ciclo di lettura figlio ritorna al padre lunghezza ultima linea letta (compreso terminatore di linea)*/
            exit(i);
        }
    }

    /* processo padre */
    //chiudo tutte le pipe in scrittura (padre deve solo leggere)
    for(n = 0; n < N; n++){
        close(piped[n][1]);
    }

    for(i = 0; i < L; i++){/*padre rispettando l'ordine delle linee*/
        for(n = 0; n < N; n++){/*e quindi dei file*/
            nr = read(piped[n][0], &l, sizeof(l)); /*legge la lunghezza della linea inviata dal figlio*/
            if(nr != sizeof(l)){
                printf("DEBUG:PADRE HA LETTO NUMERO DI BYTE SBAGLIATI %i dal figlio %d per la lunghezza\n", nr, n);
            }
            nr = read(piped[n][0], linea, l*sizeof(char)); /*e usa questa informazione per leggere la linea stessa*/
            if(nr != l*sizeof(char)){
                printf("DEBUG:PADRE HA LETTO NUMERO DI BYTE SBAGLIATI %i dal figlio %d per la linea\n", nr, n);
            }
            write(fd, linea, l*sizeof(char)); /*a questo punto padre scrive la linea sul file creato precedentemente*/
        }
    }

    /*padre aspetta i figli*/
    for(n = 0; n < N; n++){
       
        if ((pid = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(10);
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