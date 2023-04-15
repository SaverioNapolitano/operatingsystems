#!/bin/sh

#file comandi principale

#controllo che siano passati esattamente 2 parametri
case $# in
2)	echo OK passati 2 parametri;;
*)	echo Errore: necessari 2 parametri, non $#
	exit 1;;
esac

#controllo che il primo parametro sia nome assoluto directory
case $1 in
/*)	#se inizia per / nome assoluto, controllo sia directory
	if test ! -d $1 -o ! -x $1
	then	echo Errore: $1 non directory o non attraversabile
		exit 2
	fi;;
*)	#altrimenti non nome assoluto
	echo Errore: $1 non nome assoluto
	exit 3;;
esac

#controllo che il secondo parametro sia nome relativo semplice directory
case $2 in
*/*)	#se contiene uno / in qualunque posizione non nome relativo semplice
	echo Errore: $2 non nome relativo semplice
	exit 4;;
*)	;; #altrimenti nome relativo semplice
esac

#controlli finiti, settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

#creazione file temporaneo
> /tmp/files$$

#invocazione file comandi ricorsivo passando come parametri la gerarchia, il nome semplice e il file temporaneo
FCR.sh $1 $2 /tmp/files$$

#variabile che contiene lista file trovati
files=

#salvo i file trovati in una lista
for i in `cat /tmp/files$$`
do
	files="$files $i"
done

#invocazione parte C con parametri i file trovati
echo Invocazione parte C con parametri $files

#rimozione file temporaneo
rm /tmp/files$$
	
