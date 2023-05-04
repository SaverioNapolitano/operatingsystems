#include <stdio.h>	// Includo la libreria per la funzione printf
#include <stdlib.h>	// Includo la libreria per la funzione exit
#include <unistd.h>	// Includo la libreria per la funzione close e fork


int main(int argc, char** argv) {
    int n;
    printf("Inserire un numero intero (0 per non fare invocazione ricorsiva)\n");
    scanf("%i", &n);
    if(n != 0){
        printf("Invocazione ricorsiva\n");
        execl("prova", "prova", (char *) 0);
    }

    exit(0);
}