#!/bin/sh

#file comandi principale

#controllo che siano passati almeno 3 parametri
case $# in
0|1|2)	echo Errore: necessari almeno 3 parametri, $# sono pochi
	exit 1;;
*)	echo OK, passati $# parametri;;
esac

#variabile per memorizzare i primi N parametri
params=

#variabile per memorizzare ultimo parametro
S=

#contatore che mi dice a quale parametro sono arrivato
N=1

for i
do
	#se ultimo parametro
	if test $N -eq $#
	then	#controllo che non ci siano /
		case $i in
		*/*)	echo Errore: $i non deve contenere /
			exit 2;;
		*)	#memorizzo la stringa
			S=$i;;
		esac
	else	#non ultimo parametro
		#controllo che sia nome assoluto directory
		case $i in
		/*)	#nome assoluto, controllo sia directory
			if test ! -d $i -o ! -x $i
			then	echo Errore: $i non directory o non traversabile
				exit 3
			fi;;
		*)	#non nome assoluto
			echo Errore: $i non nome assoluto;;
		esac
		#se arrivo qui parametro valido, lo memorizzo
		params="$params $i"
	fi
	N=`expr $N + 1`
done

#settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

#creazione file temporaneo
> /tmp/conta$$

#invocazione ricorsiva (scorro la lista delle gerarchie)
for i in $params	
do
	FCR.sh $i $S /tmp/conta$$
done

#variabile che mi dice quanti file ho contato globalmente
NF=`wc -l < /tmp/conta$$` #prima conto tutte le linee del file
NF=`expr $NF / 2` #poi le divido per 2 perche meta sono la lunghezza in caratteri dei file
echo Numero file trovati globalmente=$NF

#variabile in cui salvo il nome del file
F=
for i in `cat < /tmp/conta$$`
do
	case $i in
	*[!0-9]*)	#se non numero quindi nome file lo stampo
			echo Nome assoluto del file=$i
			#e lo memorizzo
			F=$i;;
	*)		#se numero
			echo Lunghezza in caratteri del file=$i
			echo Vuoi ordinare il file? y/n
			read risposta
			case $risposta in
			y*|Y*|S*|s*)	echo $F ordinato
					#ordino il file e lo mostro su standard output
					sort -f $F;;
			*)		echo Non ordino $F;;
			esac;;
	esac
done

#rimozione file temporaneo
rm /tmp/conta$$	
