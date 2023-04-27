#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

int main(){
    int pidfiglio; /*contiene il PID del processo figlio in caso di successo della wait*/
    int status;/*contiene il valore della exit del figlio in caso di successo o il valore del segnale in caso di insuccesso*/
    printf("PID del processo corrente (padre) = %d\n", getpid());
    pidfiglio = wait(&status);
    if(pidfiglio < 0){
        /*non ci sono figli da attendere*/
        printf("Errore: non ci sono figli da attendere\n");
        exit(1);
    }
    exit(0);
}