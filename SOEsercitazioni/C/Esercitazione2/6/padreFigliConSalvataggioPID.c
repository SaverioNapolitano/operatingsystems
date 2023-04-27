#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>

int mia_random(int n){
    int casuale;
    casuale = rand() % n;
    return casuale;

}
int main(int argc, char **argv){

    int N; /*contiene la rappresentazione numerica del parametro passato*/
    int pid; /*pid per fork*/
    int i; /*indice per figli*/
    int pidFiglio, status, ritorno; /*per wait e valore di ritorno figli*/
    int *pidBuffer; /*array che conterra' i pid dei figli*/
    /*controllo che sia passato esattamente un parametro*/
    if(argc != 2){
        printf("Errore: necessario esattamente un parametro (numero intero fra 0 e 155) ma argc = %d\n", argc);
        exit(1);
    }

    /*controllo che il parametro sia un numero positivo e strettamente minore di 255*/
    N = atoi(argv[1]);
    if(N <= 0 || N >= 155){
        /*parametro non valido*/
        printf("Errore: %s non maggiore di 0 o non minore di 155\n", argv[1]);
        exit(2);
    }
    /*alloco la memoria necessaria a contenere i pid dei figli*/
    pidBuffer = malloc(N * sizeof(int));
    if(pidBuffer == NULL){
        printf("Errore nella malloc\n");
        exit(5);
    }
    /*se arrivo qui il parametro e' valido*/
    printf("PID del processo corrente (padre) = %d\nNumero passato come parametro = %d\n", getpid(), N);
    /*creazione figli*/
    for(i = 0; i < N; i++){
        if((pid = fork()) < 0){
            /*fork fallita*/
            printf("Errore nella fork\n");
            exit(3);
        }
        if(pid == 0){
            /*figlio*/
            printf("Indice di ordine del figlio = %d\nPID del figlio = %d\n", i, getpid());
            srand(time(NULL));
            int random = mia_random(101 + i);
            exit(random);
        }
        /*padre*/
        pidBuffer[i] = pid;

    } /*fine for*/
    for(i = 0; i < N; i++){
        pidFiglio = wait(&status);
        if(pidFiglio < 0){
            printf("Errore: non c'erano figli da attendere\n");
            exit(4);
        }
        if((status & 0xFF) != 0){
            printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        }
        else{
            ritorno = (int)((status >> 8) & 0xFF);
            int j;
            for(int z = 0; z < N; z++){
                if(pidBuffer[z] == pidFiglio){
                    j = z;
                }
            }
            printf("Il figlio con pid=%d e indice d'ordine %d ha ritornato %d\n", pidFiglio, j, ritorno);
        }

    }
    /*libero la memoria allocata*/
    free(pidBuffer);
    exit(0);

}