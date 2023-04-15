#!/bin/sh

#file comandi principale

#controllo di avere almeno 3 parametri
case $# in
0|1|2)	echo Errore: necessari almeno 3 parametri, $# sono pochi
	exit 1;;
*)	echo OK $# parametri passati;;
esac

#controllo che tutti i parametri siano nomi assoluti directory
for G
do
	if test ! -d $G -o ! -x $G
	then	echo Errore: $G non directory o non traversabile
		exit 2
	fi
done

#settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

#creazione file temporaneo
> /tmp/nomiAssoluti

#invocazione file comandi ricorsivo
for G
do
	#si passano come parametri la directory corrente e il nome assoluto del file temporaneo
	FCR.sh $G /tmp/nomiAssoluti
done

#variabile per memorizzare numero file trovati
N=`wc -l < /tmp/nomiAssoluti`

echo Numero di file trovati=$N
echo Invocazione parte C con parametri `cat /tmp/nomiAssoluti`
