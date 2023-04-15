#!/bin/sh

#file comandi principale

#controllo che il numero di parametri sia almeno 3 (controllo lasco)
case $# in
0|1|2)	echo Errore: necessari almeno 3 parametri, $# sono pochi
	exit 1;;
*)	echo OK passati $# parametri;;
esac

#controllo che il primo parametro sia un numero X tale che 0 < X < 500
case $1 in
*[!0-9]*)	echo Errore: $1 non numero o non positivo
		exit 2;;
*)		#se arrivo qui so che e\' numero
		#controllo che sia > 0 e < 500
		if test $1 -le 0 -o $1 -ge 500
		then	#se arrivo qui significa che il numero non va bene
			echo Errore: $1 e\' zero oppure maggiore di 500
			exit 3
		fi;;
esac

#se arrivo qui significa che il numero e\' valido, lo memorizzo in una variabile
X=$1

#facendo lo shift ora la lista dei parametri contiene solo le gerarchie da controllare
shift

#posso scorrerla con un for
for G
do
	case $G in
	/*)	#nome assoluto, ora devo verificare che sia una directory
		if test ! -d $G -o ! -x $G
		then	echo Errore: $G non directory o non traversabile
			exit 4
		fi;;
	*)	echo Errore: $G non nome assoluto
		exit 5;;
	esac
done

#settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

#creazione file temporaneo
> /tmp/files$$

#chiamata al file comandi ricorsivo
for G
do
	FCR.sh $G $X /tmp/files$$
done

#se sono stati trovati almeno due file (in /tmp/files$$ viene memorizzato un fie per ogni riga)
#quindi contando le linee si conta il numero di file trovati
if test `wc -l < /tmp/files$$` -ge 2
then	echo Invocazione parte C con parametri `cat /tmp/files$$`
fi

#rimozione file temporaneo
rm /tmp/files$$
