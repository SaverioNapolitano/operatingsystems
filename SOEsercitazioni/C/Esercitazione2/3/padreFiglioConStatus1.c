#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
int mia_random(int n){
    int casuale;
    casuale = rand() % n;
    return casuale;

}
int main(){
    int pid; /*contiene il valore di ritorno della fork*/
    int pidfiglio; /*contiene in caso di successo il PID del processo generato dalla fork*/
    int status;/*contiene in caso di successo il valore di ritorno della exit del figlio nel byte alto*/
    int ritorno; /*contiene valore di ritorno della exit del figlio*/
    printf("PID del processo corrente (padre) = %d\n", getpid());
    pid = fork();
    if(pid < 0){
        /*fork fallita*/
        printf("Errore nella fork\n");
        exit(1);
    }
    if(pid == 0){
        /*figlio*/
        printf("PID del processo corrente (figlio) = %d\nPID del processo padre = %d\n", getpid(), getppid());
        srand(time(NULL));
        int random = mia_random(100);
        exit(random);

    }
    pidfiglio = wait(&status);
    if(pidfiglio < 0){
        /*non ci sono figli da attendere*/
        printf("Errore: il processo corrente non aveva figli\n");
        exit(2);
    }
    /*altrimenti c'era figlio che ha terminato*/
    if((status & 0xFF) != 0){
        /*terminazione anomala*/
        printf("Il processo e' terminato in modo anormale\n");
        exit(3);
    }
    /*altrimenti terminazione normale*/
    /*aspetto secondo figlio*/
    pidfiglio = wait(&status);
    if(pidfiglio < 0){
        /*non ci sono figli da attendere*/
        printf("Errore: il processo corrente non aveva figli\n");
        exit(2);
    }
    ritorno = (int)((status >> 8) & 0xFF); /*isolo byte alto*/
    printf("PID del processo figlio = %d\nValore ritornato = %d\n", pid, ritorno);
    exit(0);
}