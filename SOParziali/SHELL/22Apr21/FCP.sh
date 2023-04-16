#!/bin/sh

#file comandi principale

#controllo che i parametri siano almeno 4
case $# in
0|1|2|3)	echo Errore: necessari almeno 4 parametri, $# sono pochi
		exit 1;;
*)		echo OK passati $# parametri;;
esac

#controllo che il primo parametro sia un numero > 0
case $1 in
*[!0-9]*)	#se fa match allora non numero o non positivo
		echo Errore: $1 non numero o non positivo
		exit 2;;
*)		#numero positivo, controllo che non sia 0
		if test $1 -eq 0
		then	echo Errore: $1 e\' zero
			exit 3
		fi;;
esac

#se arrivo qui parametro valido, lo salvo
W=$1

#controllo che il secondo parametro non contenga /
case $2 in
*/*)	echo Errore: $2 non deve contenere /
	exit 4;;
*)	;;
esac

#se arrivo qui parametro valido, lo salvo
S=$2

#shifto cosi nella lista dei parametri ho solo le gerarchie da controllare
shift 2

#posso scorrerla con un for
for G
do
	#controllo che sia nome assoluto directory
	case $G in
	/*)	#nome assoluto, controllo che sia directory
		if test ! -d $G -o ! -x $G
		then	echo Errore: $G non directory o non traversabile
			exit 5
		fi;;
	*)	#non nome assoluto
		echo Errore: $G non nome assoluto
		exit 6;;
	esac
done

#settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

#creazione file temporaneo
> /tmp/nomiAssoluti

#invocazione file comandi ricorsivo
for G
do
	FCR.sh $G $S /tmp/nomiAssoluti
done

#salvo numero di directory trovate globalmente
ND=`wc -l < /tmp/nomiAssoluti`

#se tale numero maggiore o uguale del parametro passato
if test $ND -ge $W
then	echo $USER dammi un numero intero fra 1 e $W
	read X
	#controllo che sia un numero
	case $X in
	*[!0-9]*)	echo Errore: $X non numero o non positivo
			exit 7;;
	*)		#controllo che sia compreso fra 1 e $W
			if test $X -lt 1 -o $X -gt $W
			then	echo Errore: $X deve essere compreso fra 1 e $W
				exit 8
			fi;;
	esac
fi

#se arrivo qui numero valido
echo Nome assoluto directory $X
head -$X /tmp/nomiAssoluti | tail -1 

echo contenuto totale file
cat /tmp/nomiAssoluti
#rimozione file temporaneo
rm /tmp/nomiAssoluti
