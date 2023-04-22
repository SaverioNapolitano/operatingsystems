#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv){

    /*controllo che siano passati esattamente 23caratteri*/
    if(argc != 4){
        printf("Errore nel numero di parametri passati dato che argc = %d\n", argc);
        exit(1);
    }

    /*se arrivo qui numero di parametri passati e' corretto*/

    /*controllo che il primo parametro sia il nome di un file usando la primitiva open*/
    int fd = open(argv[1], O_RDWR);

    /*se fd < 0 la open ha fallito*/
    if(fd < 0){
        printf("Errore nell'apertura del file\n");
        exit(2);
    }

    /*controllo che il secondo parametro sia un singolo carattere*/
    /*se la sua lunghezza non e' uno il parametro non e' valido*/
    if(strlen(argv[2]) != 1){
        printf("Errore: %s non singolo carattere\n", argv[2]);
        exit(3);
    }

    /*controllo che il terzo parametro sia un singolo carattere*/
    /*se la sua lunghezza non e' uno il parametro non e' valido*/
    if(strlen(argv[3]) != 1){
        printf("Errore: %s non singolo carattere\n", argv[3]);
        exit(5);
    }

    /*se arrivo qui parametri corretti*/
    int nread; /*variabile che mi dice quanti byte ho letto*/
    int nwrite; /*variabile che mi dice quanti byte ho scritto*/
    char buff; /*variabile che contiene il carattere letto*/
    char car = argv[3][0]; /*variabile che contiene il carattere da inserire*/

    /*fino a quando ci sono caratteri da leggere*/
    while((nread = read(fd, &buff, 1))>0){
        /*se il carattere letto e' uguale a quello cercato*/
        if(buff == argv[2][0]){
            /*sposto il file pointer indietro di 1*/
            lseek(fd, -1, SEEK_CUR);
            /*e sostituisco il carattere con uno spazio*/
            nwrite = write(fd, &car, 1);
            /*se nwrite diverso da 1 ho avuto un errore*/
            if(nwrite != 1){
                printf("Errore nella scrittura\n");
                exit(4);
            }
        }
    }

    /*quando ho finito di leggere e scrivere chiudo il file*/
    close(fd);
    exit(0);
}