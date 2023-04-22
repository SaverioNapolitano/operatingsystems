#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>


int main(int argc, char** argv){

    /*controllo che siano passati esattamente due parametri*/
    if(argc != 3){
        printf("Errore: passato un numero di parametri sbagliato dato che argc = %d\n", argc);
        exit(1);
    }

    /*se arrivo qui numero di parametri passati e' corretto*/
    /*controllo che il primo parametro sia il nome di un file con la primitiva open*/
    int fd = open(argv[1], O_RDONLY);
    /*se fd < 0 la open ha fallito*/
    if(fd < 0){
        printf("Errore nell'apertura del file\n");
        exit(2);
    }
    /*controllo che il secondo parametro sia un numero > 0*/
    int n = atoi(argv[2]);
    if(n <= 0){
        printf("Errore: %s non numero o non positivo\n", argv[2]);
        exit(3);
    }
    /*se arrivo qui il tipo dei parametri e' corretto*/
    int nread; /*variabile che mi dice quanti caratteri ho letto*/
    char buffer[256]; /*variabile che contiene la riga corrente del file*/
    int j = 0; /*indice che scorre gli elementi del buffer*/
    int trovata = 0; /*variabile che mi dice se ho trovato la linea cercata*/
    int linee = 1; /*variabile che mi dice quante linee ho scorso*/
    /*fino a quando ci sono caratteri da leggere*/
    while((nread = read(fd, &(buffer[j]), 1)) != 0){
        /*se il carattere letto e' \n*/
        if(buffer[j] == '\n'){
            /*incremento il contatore delle linee*/
            linee++;
            /*se ho letto oltre la linea cercata*/
            if(linee > n){
                /*la linea cercata esiste, aggiorno la variabile trovata ed esco*/
                trovata = 1;
                break;
            }
            /*azzero il buffer*/
            for(int i = 0; i <= j; i++){
                buffer[i] = 0;
            }
            /*azzero il contatore*/
            j = -1;
        }
        /*incremento il contatore*/
        j++;
    }
    /*se trovata non ha cambiato valore allora la linea cercata non esiste*/
    if(trovata == 0){
        printf("Non esiste la linea %d nel file %s\n", n, argv[1]);
    }
    else{
        /*altrimenti ho trovato la linea*/
        buffer[j] = 0; /*inserisco il terminatore*/
        printf("La linea %d del file %s:\n", n, argv[1]);
        printf("%s\n", buffer);
    }
    /*chiudo il file*/
    close(fd);
    exit(0);
}