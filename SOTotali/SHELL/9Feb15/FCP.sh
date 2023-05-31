#!/bin/sh

#file comandi FCP.sh

#controllo numero parametri
case $# in
2)	echo Numero parametri ok;;
*)	echo Errore: voglio 2 parametri, non $#
	exit 1;;
esac
#fine controllo numero parametri

#controllo tipo parametri

#controllo primo parametro sia nome assoluto directory
case $1 in
/*)	echo $1 nome assoluto directory;;
*)	echo Errore: $1 non nome assoluto directory #se non inizia per / non nome assoluto
	exit 2;;
esac

#controllo secondo parametro sia numero intero strettamente positivo
case $2 in
*[!0-9]*)	echo Errore: $2 non numero #se contiene carattere diverso da cifra non numero
		exit 3;;
*)		echo DEBUG-- $2 numero
		if test $2 -eq 0 #controllo se numero strettamente positivo
		then	echo Errore: voglio numero strettamente positivo, non 0
			exit 4
		fi;;
esac

#fine controllo tipo parametri

#settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

#creazione file temporaneo
>/tmp/files$$

#invocazione parte ricorsiva
FCR.sh $* /tmp/files$$

echo DEBUG-- `cat /tmp/files$$`

#rimozione file temporaneo
rm /tmp/files$$
