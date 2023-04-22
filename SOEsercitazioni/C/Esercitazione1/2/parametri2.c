#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
int main(int argc, char **argv){
/*controllo che siano passati esattamente tre parametri*/
if(argc!=4){
	printf("Errore: numero di parametri passati sbagliato dato che argc= %d\n", argc);
	exit(1);
}
/*se arrivo qui numero di parametri passati e' corretto*/

/*controllo che il primo parametro sia il nome di un file usando la primitiva open*/
int fd = open(argv[1], O_RDWR);

/*se la open ha fallito fd ha valore negativo*/
if(fd < 0){
	printf("Errore nell'apertura del file\n");
	exit(1);
}
/*se arrivo qui il parametro e' valido, quindi chiudo il file*/
int retval = close(fd);
if(retval < 0){
	printf("Errore nella chiusura del file\n");
	exit(4);
}
/*controllo che il secondo parametro sia un numero maggiore di 0 usando la primitva atoi*/
int n = atoi(argv[2]);
if(n<=0){
	printf("Errore: %i non numero positivo\n", n);
	exit(2);
}

/*controllo che il terzo parametro sia un singolo carattere usando la funzione strlen*/
if(strlen(argv[3])!=1){
	/*se la lunghezza della stringa che rappresenta il secondo parametro non e' uno esco*/
	printf("Errore: %s non singolo carattere\n", argv[3]);
	exit(3);
}

/*se arrivo qui significa che i parametri sono corretti*/
printf("Nome eseguibile = %s\n", argv[0]);

/*stampo il primo parametro*/
printf("Il primo rappresenta il nome di un file ed e' %s\n", argv[1]);

/*stampo il secondo parametro*/
printf("Il secondo parametro rappresenta un numero intero > 0 ed e' %d\n", n);

/*stampo il terzo parametro*/
printf("Il terzo parametro rappresenta un singolo carattere ed e' %s\n", argv[3]);

exit(0);
}
