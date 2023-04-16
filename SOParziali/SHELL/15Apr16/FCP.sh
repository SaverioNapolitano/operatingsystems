#!/bin/sh

#file comandi principale

#controllo che siano passati almeno 3 parametri
case $# in
0|1|2)	echo Errore: necessari almeno 3 parametri, $# sono pochi
	exit 1;;
*)	echo OK passati $# parametri;;
esac

#variabile in cui salvo i primi N parametri
params=
#variabile in cui salvo ultimo parametro
X=
#contatore che mi dice a quale parametro sono
N=1

for i
do
	#se sono arrivato all'ultimo parametro
	if test $N -eq $#
	then	#controllo sia numero intero strettamente positivo
		case $i in
		*[!0-9]*)	#se contiene almeno un carattere non numerico in qualunque posizione
				echo Errore: $i non numero o non positivo
				exit 2;;
		*)		#numero positivo, controllo non sia 0
				if test $i -eq 0
				then	echo Errore: $i corrisponde a zero
					exit 3
				fi;;
		esac
		#se arrivo qui parametro valido, lo salvo
		X=$i
	#se non ultimo parametro
	else 	#controllo sia nome assoluto directory
		case $i in
		/*)	#nome assoluto, controllo sia directory
			if test ! -d $i -o ! -x $i
			then	echo Errore: $i non directory o non traversabile
				exit 4
			fi;;
		*)	#non nome assoluto
			echo Errore: $i non nome assoluto
			exit 5;;
		esac
		#se arrivo qui parametro valido, lo salvo
		params="$params $i"
	fi
	#incremento contatore
	N=`expr $N + 1`
done	

#controlli finiti, ora settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

#creazione file temporaneo
> /tmp/conta$$

#passo ora alle N fasi, scorro la lista in cui ho memorizzato le gerarchie
for G in $params
do
	#invocazione file ricorsivo passando come parametro la gerarchia corrente, il numero memorizzato in X e il file temporaneo
	FCR.sh $G $X /tmp/conta$$
done

echo Trovati globalmente `wc -l < /tmp/conta$$` directory

#per ogni directory trovata
for D in `cat /tmp/conta$$`
do
	echo Nome assoluto directory=$D
	#per ogni file contenuto nella directory 
	for F in `ls $D`
	do
		echo Nome assoluto file=$D/$F
		echo Linea $X a partire dalla fine del file
		tail -$X $D/$F | head -1
	done
done

#rimozione file temporaneo
rm /tmp/conta$$

