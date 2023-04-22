#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv){
    /*controllo che siano passati esattamente due parametri*/
    if(argc != 3){
        printf("Errore: numero di parametri passati sbagliato dato che argc = %d\n", argc);
        exit(1);
    }
    /*se arrivo qui il numero di parametri passati e' corretto*/
    /*controllo che il primo parametro sia un nome di file usando la primitiva open*/
    int fd = open(argv[1], O_RDONLY);
    /*se fd < 0 la open ha fallito*/
    if(fd < 0){
        printf("Errore nell'apertura del file\n");
        exit(2);
    }
    /*controllo che il secondo parametro sia un numero intero > 0*/
    int n = atoi(argv[2]);
    /*se il numero e' minore o uguale a zero non va bene*/
    if(n <= 0){
        printf("Errore: %i non maggiore di 0\n", n);
        exit(3);
    }
    /*se arrivo qui i parametri sono corretti*/
    /*variabile che mi dice a che multiplo sono*/
    int multiplo = 1;
    /*calcolo la lunghezza del file*/
    long int lenght = lseek(fd, 0, SEEK_END);
    /*riposiziono il file pointer all'inizio del file*/
    long int cur = lseek(fd, 0L, 0);
    /*variabile in cui memorizzo il carattere letto*/
    char c;
    /*fino a quando la posizione e' valida, siccome il file pointer si sposta avanti di 1
    rispetto alla posizione raggiunta faccio -1*/
    while((cur = lseek(fd, multiplo*n-1, SEEK_SET))<lenght-1){
        /*leggo il carattere alla posizione corrente*/
        int nread = read(fd, &c, 1);
        /*se nread diverso da 1 la lettura ha fallito*/
        if(nread != 1){
            printf("Errore nella lettura");
            exit(4);
        }
        /*altrimenti ho letto correttamente il carattere*/
        printf("Il multiplo %d-esimo all'interno del file %s e' %c\n", multiplo, argv[1], c);
        multiplo++;
    }

    /*chiudo il file*/
    int retval = close(fd);
    if(retval < 0){
        printf("Errore nella chiusura del file\n");
        exit(5);
    }
    exit(0);
}