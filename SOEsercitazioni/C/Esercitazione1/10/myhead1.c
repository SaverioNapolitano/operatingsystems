#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char **argv){
    /*controllo che sia passato esattamente un parametro*/
    if(argc != 2){
        printf("Errore nel numero dei parametri passati dato che argc = %d\n", argc);
        exit(1);
    }
    /*se arrivo qui numero di parametri passati e' corretto*/
    /*controllo che il primo carattere del parametro sia -*/
    if(argv[1][0] != '-'){
        printf("Errore: primo carattere deve essere -\n");
        exit(2);
    }
    /*calcolo la lunghezza della stringa*/
    size_t len = strlen(argv[1]);
    
    
    
    /*alloco dinamicamente un array di char*/
    char *s = malloc(len-1);
    size_t j = 0; /*contatore per s*/
    /*copio nel vettore allocato dinamicamente tutti i caratteri della stringa tranne il primo*/
    for(size_t i = 1; i < len; i++){
        s[j]=argv[1][i];
        j++;
    }
    /*converto la stringa ottenuta in numero*/
    int n = atoi(s);
    /*libero la memoria occupata dalla stringa allocata dinamicamente */
    free(s);
    /*controllo che il numero ottenuto sia strettamente positivo*/
    if(n <= 0){
        printf("Errore: %s non numero o non positivo\n", argv[1]);
        exit(3);
    }
    /*se arrivo qui il parametro e' corretto*/
    int linee = 1; /*variabile che mi dice quante linee ho letto*/
    /*variabile che mi dice la massima capienza della stringa*/
    int dim = 1;
    char *stringa = malloc(dim); /*variabile in cui memorizzare la stringa in input*/
    int i = 0; /*contatore per stringa*/
    int nread = 0; /*mi dice quanti caratteri ho letto*/
    /*fino a quando il numero di linee non supera n*/
    while(linee <= n){
         /*salvo il carattere letto sulla stringa*/
        nread = read(0, &stringa[i], 1);
        /*se nread ha valore < 0 ho avuto un errore*/
        if(nread < 0){
            printf("Errore durante la lettura\n");
            exit(5);
        }
        /*se nread = 0 sono arrivato alla fine del file*/
        if(nread == 0){
            /*esco dal ciclo*/
            break;
        }
        /*se il carattere letto e' \n*/
        if(stringa[i] == '\n'){
            /*termino la stringa*/
            stringa[i+1]=0;
            /*stampo la stringa su standard output*/
            printf("%s", stringa);
            /*inizializzo il contatore di stringa a -1 siccome dopo viene incrementato*/
            i = -1;
            /*incremento il contatore di linee*/
            linee++;
        }
        /*se ho raggiunto il limite della stringa*/
        if(i == dim - 1){
            /*aumento la dimensione*/
            dim*=2;
            /*rialloco la stringa*/
            stringa = realloc(stringa, dim);
        }
        /*incremento contatore stringa*/
        i++;
    }
    /*finito di leggere libero la memoria allocata*/
    free(stringa);
    exit(0);
}