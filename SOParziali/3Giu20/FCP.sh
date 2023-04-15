#!/bin/sh

#file comandi principale

#controllo che i parametri passati siano almeno 3
case $# in
0|1|2)	echo Errore: necessari almeno 3 parametri, $# sono pochi
	exit 1;;
*)	echo OK passati $# parametri;;
esac

#controllo che il primo parametro sia un singolo carattere
case $1 in
?)	#se fa match con ? allora singolo carattere
	#lo salvo in una variabile
	C=$1;;
*)	#se non singolo carattere
	echo Errore: $1 deve essere singolo carattere
	exit 2;;
esac

#shiftando i parametri ora la lista comprende solo le gerarchie da controllare
shift 

#posso scorrerla con un for
for i
do
	#controllo che sia nome assoluto directory
	case $i in
	/*)	#nome assoluto, controllo che sia directory
		if test ! -d $i -o ! -x $i
		then	echo Errore: $i non directory o non traversabile
			exit 3
		fi;;
	*)	#non nome assoluto
		echo Errore: $i non nome assoluto
		exit 4;;
	esac
done

#settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

#creazione file temporaneo
> /tmp/nomiAssoluti

for i 
do
	FCR.sh $i $C /tmp/nomiAssoluti
done

#memorizzo in una variabile il numero di directory trovate globalmente
ND=`wc -l < /tmp/nomiAssoluti`

echo Trovate $ND directory

for i in `cat < /tmp/nomiAssoluti`
do
	echo Nome assoluto directory $i
	echo Elena, vuoi visualizzare il contenuto di $i compresi gli elementi nascosti? y/n
	read answer
	case $answer in
	y*|Y*|s*|S*)	ls -l -a $i;;
	*)		;;
	esac
done

#rimozione file temporaneo
rm /tmp/nomiAssoluti
