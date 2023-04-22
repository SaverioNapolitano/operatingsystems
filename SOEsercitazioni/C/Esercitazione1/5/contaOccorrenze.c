#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
int main(int argc, char **argv){
	/*controllo che siano passati esattamente due parametri*/
	if(argc!=3){
		printf("Errore: numero di parametri passati sbagliato dato che argc= %d\n", argc);
		exit(1);
	}
	/*se arrivo qui numero di parametri passati corretto*/
	/*controllo che primo parametro sia nome di un file usando la primitva open*/
	int fd = open(argv[1], O_RDONLY);
	/*se fd ha valore negativo la open ha fallito*/
	if(fd < 0){
		printf("Errore nell'apertura del file\n");
		exit(2);
	}
	/*controllo che il secondo parametro sia un singolo carattere*/
	if(strlen(argv[2])!=1){
		/*se la lunghezza della stringa che rappresenta il secondo parametro e' maggiore di 1 esco*/
		printf("Errore: %s non singolo carattere\n", argv[2]);
		exit(3);
	}
	/*se arrivo qui parametri corretti*/
	long int count = 0; /* variabile che mi dice il numero di occorrenze trovate*/
	char buf; /* variabile che contiene l'ultimo carattere letto*/
	int nread;/* variabile che mi dice quanti caratteri ho letto o se ho avuto errori*/
	while((nread = read(fd, &buf, 1)) == 1){
		/*se entro nel ciclo allora ho letto un carattere e l'ho messo in buf, controllo se e' uguale al carattere che sto cercando*/
		if(buf==argv[2][0]){
			/*se entro qui il carattere e' lo stesso, incremento il contatore*/
			count++;
		}
	}
	/*se arrivo qui ho finito di leggere il file*/
	printf("Le occorrenze del carattere %s nel file %s sono %ld\n", argv[2], argv[1], count); 
	exit(0);
}
