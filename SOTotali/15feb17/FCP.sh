#!/bin/sh

#file comandi FCP.sh

#per prima cosa controllo numero parametri
#controllo che i parametri passati siano esattamente 2 (controllo stretto)
case $# in
2)	echo Numero parametri passati ok;;
*)	echo Errore: voglio 2 parametri, non $# #qualsiasi numero diverso da 2 non va bene
	exit 1;;
esac

#controllo numero parametri finito, ora controllo tipo parametri
#controllo che il primo parametro sia nome assoluto directory
case $1 in
/*)	echo $1 nome assoluto directory;;
*)	echo Errore: $1 non nome assoluto directory #se non inizia per / non puo essere nome assoluto 
	exit 2;;
esac

#controllo che il secondo parametro sia un numero intero strettamente positivo
case $2 in
*[!0-9]*)	echo Errore: $2 non numero #se contiene un carattere diverso da una cifra non numero
		exit 3;;
*)		echo $2 numero #altrimenti numero, ma potrebbe essere zero
		if test $2 -eq 0
		then	echo Errore: $2 non positivo
			exit 4
		fi;;
esac

#controllo tipo parametri finito
#settaggio ed export variabile PATH
PATH=`pwd`:$PATH
export PATH

#invocazione del file comandi ricorsivo FCR.sh
FCR.sh $*
