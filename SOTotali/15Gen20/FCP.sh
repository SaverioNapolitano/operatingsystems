#!/bin/sh 

#file comandi principale

#controllo che siano passati almeno 3 parametri
case $# in
0|1|2)	echo Errore: necessari almeno 3 parametri, $# sono pochi
	exit 1;;
*)	echo OK, passati $# parametri;;
esac

#variabile per memorizzare il primo parametro
H=

#controllo che il primo parametro sia un numero strettamente positivo
case $1 in
*[!0-9]*)	#se contiene in qualunque posizione un carattere non numerico
		echo Errore: $1 non numero o non positivo
		exit 2;;
*)		#numero positivo, controllo non sia 0
		if test $1 -eq 0
		then	echo Errore: $1 corrisponde a zero
			exit 3
		fi;;
esac

#se arrivo qui parametro valido, lo salvo e faccio lo shift
H=$1

shift

#controllo che i parametri rimasti siano nomi assoluti directory
for i
do
	case $i in
	/*)	#se inizia per / nome assoluto, controllo che sia directory
		if test ! -d $i -o ! -x $i
		then	echo Errore: $i non directory o non traversabile
			exit 4
		fi;;
	*)	#altrimenti non nome assoluto
		echo Errore: $i non nome assoluto
		exit 5;;
	esac
done

#controlli finiti, settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

#creazione file temporaneo
> /tmp/files$$
#passo alle W fasi
for i 
do
	#invocazione file comandi ricorsivo con parametri la gerarchia corrispondente, il numero > 0 e il file temporaneo
	FCR.sh $i $H /tmp/files$$
	#variabile che contiene il numero di file trovati
	NF=`wc -l < /tmp/files$$`
	echo Numero di file trovati=$NF
	#controllo se numero di file trovati pari
	if test `expr $NF % 2` -eq 0
	then	#invocazione parte C con parametri i nomi assoluti dei file trovati
		echo Invocazione parte C con parametri `cat /tmp/files$$`
	fi
	#azzero file temporaneo
	> /tmp/files$$
done

#rimozione file temporaneo
rm /tmp/files$$
