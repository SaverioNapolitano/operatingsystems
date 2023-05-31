#!/bin/sh

#file comandi principale

#controllo che i parametri siano almeno 4
case $# in
0|1|2|3)	echo Errore: necessari almeno 4 parametri, $# sono pochi
		exit 1;;
*)		echo OK passati $# parametri;;
esac

#variabile che mi dice la posizione del parametro
pos=1
#variabile in cui memorizzo il penultimo parametro
H=
#variabile in cui memorizzo ultimo parametro
M=

#controllo tipo parametri
for i
do
	#se sono arrivato al penultimo parametro
	if test $pos -eq `expr $# - 1`
	then	#memorizzo il suo valore 
		H=$i
		echo $H
	fi
	#se sono arrivato all'ultimo parametro
	if test $pos -eq $#
	then	#memorizzo il suo valore
		M=$i
		echo $M
	fi
	#se sono arrivato al penultimo o ultimo parametro 
	if test $pos -ge `expr $# - 1`
	then 	#controllo che sia  numero intero strettamente positivo
		case $i in
		*[!0-9]*)	echo Errore: $i non numero o non positivo
				exit 2;;
		*)		if test $i -eq 0
				then	echo Errore: $i e\' zero
					exit 3
				fi;;
		esac
	#altrimenti controllo che sia nome assoluto directory
	else	case $i in
		/*)	#se non directory o non traversabile
			if test ! -d $i -o ! -x $i
			then	echo Errore: $i non directory o non traversabile
				exit 4
			fi;;
		*)	#se non inizia per / non nome assoluto
			echo Errore: $i non nome assoluto
			exit 5;;
		esac
	fi
	#aggiorno contatore
	pos=`expr $pos + 1` 
done

#riporto il contatore al suo valore iniziale
pos=1
#settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

for i
do
	#se sono arrivato al penultimo parametro significa che ho esplorato tutte le gerarchie
	echo pos=$pos actual=`expr $# - 1`
	if test $pos -eq `expr $# - 1`
	then	#quindi esco dal ciclo
		break
	fi
	#altrimenti invoco il file comandi ricorsivo con parametri directory corrente e i due numeri strettamente positivi
	FCR.sh $i $H $M
	pos=`expr $pos + 1`
done
