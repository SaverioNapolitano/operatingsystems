#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv){

/* controllo che sia passato almeno un parametro */
if(argc < 2){
	printf("Errore: numero di parametri passati sbagliato dato che argc= %d\n", argc);
	exit(1);
}

/*se arrivo qui il numero di parametri passati e' corretto*/
printf("Nome eseguibile= %s\n", argv[0]);

/*stampo il nome di tutti i parametri passati*/
for(int i=0; i<argc-1; i++){
	printf("Parametro %d= %s\n", i+1, argv[i+1]);
}

exit(0);
}
