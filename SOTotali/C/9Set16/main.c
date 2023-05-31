#include <stdio.h>	// Includo la libreria per la funzione printf e BUFSIZ
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close, fork, famiglia exec, read, write, lseek, famiglia get, pipe,
#include <fcntl.h>	// Includo la libreria per la funzione open, creat e le relative macro
#include <sys/wait.h>	// Includo la libreria per la funzione wait
#include <stdbool.h>
//definisco il tipo pipe_t
typedef int pipe_t[2];
struct struttura
{
    char v1; /*carattere trovato*/
    long int v2; /*occorrenze del carattere*/
};
void scambia(struct struttura *x, struct struttura *y){
    char a = x->v1;
    long int b = y->v2;
    x->v1=y->v1;
    x->v2=y->v2;
    y->v1=a;
    y->v2=b;
}
void bubbleSort(struct struttura *v, int dim){
    int i; bool ordinato = false;
    while (dim>1 && !ordinato)
    {  
        ordinato = true; /* hp: è ordinato */
        for (i=0; i<dim-1; i++)
          if (v[i].v2>v[i+1].v2){
            scambia(&v[i],&v[i+1]);
            ordinato = false;
        }
        dim--; 
    }
}
int main(int argc, char** argv) {

    char c; /*carattere da cercare*/
    char cl; /*carattere letto*/
    int i; /*contatore*/
    int j; /*contatore*/
    int pidFiglio;	// memorizzo il valore di ritorno della funzione fork
    int fd; /*variabile che conterra il file descriptor del file che verrà aperto con la open */
    int status;	// La variabile usata per memorizzare quanto ritornato dalla primitiva wait
    int ritorno;	// La variabile usata per memorizzare il valore di ritorno del processo figlio
    int nr; /*valore di ritorno read*/
    int nw; /*valore di rtitorno write*/
    int pid[26]; /*array per memorizzare i pid dei figli*/
    long int occorrenze; /*occorrenze del carattere*/
    pipe_t piped[26]; /*array di pipe per comunicazione in pipeline: ogni processo legge dalla pipe i-1 e scrive sulla pipe i*/
    
    struct struttura array[26];
    
    if (argc != 2) /* controllo sul numero di parametri: devono essere in numero uguale a 1*/
    {
        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    //apro 26 pipes
    for(i = 0;i < 26;i++){
        if(pipe(piped[i])<0){
            printf("Errore nella creazione della pipe numero:%i\n",i);
            exit(2);
        }
    }

    for(i=0; i<26; i++){
        
        if ((pid[i] = fork()) < 0)	/* Il processo padre crea un figlio */
        {	/* In questo caso la fork e' fallita */
            printf("Errore durante la fork\n");
            exit(3);
        }
        
        if (pid[i] == 0)
        {	/* processo figlio */
            fd = 0;
            c = 'a' + (char)i;
            occorrenze=0L;
            for (j=0;j<26;j++)
			{
				if (j!=i)
					close (piped[j][1]);
				if ((i == 0) || (j != i-1)) // j != (i+1)%26
					close (piped[j][0]);
			}
            //controllo se il file e' accedibile
            if((fd = open(argv[1], O_RDONLY)) < 0){		/* ERRORE se non si riesce ad aprire in LETTURA il file */
                printf("Errore in apertura file %s dato che fd = %d\n", argv[1], fd);
                exit(-1);
            }
            
            while (read(fd, &cl, sizeof(char)) > 0)	/* ciclo di lettura fino a che riesco a leggere un carattere da file */
            {
                if(c==cl){
                    occorrenze++;
                }
                
            }
            if(i!=0){ /*se non è il primo figlio*/
                nr = read(piped[i - 1][0], array, 26*sizeof(struct struttura));
                if(nr != 26*sizeof(struct struttura)){
                    printf("Figlio %d ha letto un numero di byte sbagliati %d\n", i, nr);
                    exit(-2);
                }
            }
            /*inizializzo l'i-esimo campo dell'array di struct*/
            array[i].v1 = c;
            array[i].v2 = occorrenze;
            
            nw = write(piped[i][1], array, 26*sizeof(struct struttura)); /*figlio i-esimo scrive su pipe successiva l'array di struct (ultimo figlio scriverà al padre)*/
            if(nw != 26*sizeof(struct struttura)){
                printf("Figlio %d ha scritto un numero di byte sbagliati %d\n", i, nw);
                exit(-3);
            }
            exit(cl);
        }
        /* processo padre */
        
    }
    /* chiusura pipe: tutte meno l'ultima in lettura */
	for(i=0;i<26;i++)
	{
		close (piped[i][1]);
		if (i != 25) close (piped[i][0]);
	}

    nr = read(piped[25][0], array, 26*sizeof(struct struttura));
    /*if(nr != 26*sizeof(struct struttura)){
        printf("Padre ha letto un numero di byte sbagliati %d\n", nr);
        exit(4);
    }*/
    bubbleSort(array, 26);
    for(i=0; i<26;i++){
        printf("Il figlio con pid %d e indice %d ha trovato %ld occorrenze del carattere %c\n", pid[(int)array[i].v1-(int)'a'], (int)array[i].v1-(int)'a', array[i].v2, array[i].v1);
    }

    for(i=0; i<26;i++){
        
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Non e' stato creato nessun processo figlio\n");
            exit(5);
        }
        
        if ((status & 0xFF) != 0)
        {
            printf("Il processo figlio è stato terminato in modo anomalo\n");
        } else {
            ritorno = (status >> 8) & 0xFF;
            printf("Il figlio con pid %d ha ritornato %d che corrisponde al carattere %c\n", pidFiglio, ritorno, (char)ritorno);
        }
    }

    exit(0);
}