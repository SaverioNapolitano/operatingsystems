#!/bin/sh

#file comandi principale

#controllo che il numero di parametri sia almeno 3
case $# in
0|1|2)	echo Errore: almeno 3 parametri, pochi $#
	exit 1;;
*)	echo OK Passati $# parametri;;
esac

#controllo che il primo parametro sia un numero intero strettamente positivo
case $1 in
*[!0-9]*)	echo Errore: $1 non numero o non positivo
		exit 2;;
*)		if test $1 -eq 0
		then	echo Errore: voglio un numero strettamente positivo, non 0
			exit 3
		fi;;
esac

#variabile in cui memorizzo il primo parametro
X=$1

#con lo shift adesso la mia lista dei parametri contiene tutti gli elementi di prima meno il primo
shift

#controllo che questi parametri siano nomi assoluti di directory
#avendo fatto lo shift posso scorrere tutta la lista dei parametri
for i
do
	case $i in
	/*)	if test ! -d $i -o ! -x $i
		then	echo Non directory o non traversabile
			exit 4
		fi
		echo OK nome assoluto directory;;
	*)	echo Errore: non nome assoluto
		exit 5;;
	esac
done

#settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

#creazione file temporaneo
> /tmp/files$$

#invocazione parte ricorsiva
for i 
do
	#invoco il file comandi ricorsivo passando come parametri la gerarchia corrente, il numero strettamente positivo precedentemente salvato e il file temporaneo su cui salvare i nomi assoluti dei file trovati
	 FCR.sh $i $X /tmp/files$$
done

echo Il numero totale di file trovati globalmente=`wc -l < /tmp/files$$`

for i in `cat < /tmp/files$$`
do
	echo Nome assoluto file=$i
	echo Inserire un numero strettamente positivo e strettamente minore di $X
	read K
	case $K in
	*[!0-9]*)	echo Errore:$K non numero o non positivo
			rm /tmp/files$$
			exit 6;;
	*)		if test $K -eq 0 -o $K -ge $X
			then	echo Errore: $K maggiore o uguale di $X oppure e\' zero
			rm /tmp/files$$
			exit 7
			fi;;
	esac
	echo  linea $K di $i
	echo `head -$K < $i | tail -1`
done

#rimozione file temporaneo
rm /tmp/files$$
