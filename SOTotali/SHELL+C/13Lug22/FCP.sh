#!/bin/sh

#file comandi principale

#controllo che il numero di parametri sia almeno 4
case $# in
0|1|2|3)	echo Errore: necessari almeno 4 parametri, $# sono pochi
		exit 1;;
*)		echo OK passati $# parametri;;
esac

#variabile per memorizzare il primo parametro
G=
#variabile per memorizzare il secondo parametro
L=

#controllo che il primo parametro sia nome assoluto directory 
case $1 in
/*)	#nome assoluto, controllo che sia directory
	if test ! -d $1 -o ! -x $1
	then	echo Errore: $1 non directory o non traversabile
		exit 2
	fi;;
*)	#altrimenti non nome assoluto
	echo Errore: $1 non nome assoluto
	exit 3;;
esac

#se arrivo qui parametro valido, lo salvo
G=$1

#controllo che secondo parametro sia numero intero > 0
case $2 in
*[!0-9]*)	#se contiene in una qualunque posizione un carattere non numerico  
		echo Errore: $2 non numero o non positivo
		exit 4;;
*)		#altrimenti numero, controllo che sia > 0
		if test $2 -eq 0
		then	echo Errore: $2 e\' zero
			exit 5
		fi;;
esac

#se arrivo qui parametro valido, lo salvo
L=$2

#faccio lo shift
shift 2

#adesso nella lista ho solo i caratteri da controllare, posso scorrerli con un for
for C
do
	#controllo che sia singolo carattere
	case $C in
	?)	;;
	*)	echo Errore: $C non singolo carattere
		exit 6;;
	esac
done

#controlli finiti, settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

#creazione file temporaneo
> /tmp/nomiAssoluti

#invocazione file comandi ricorsivo con parametri la gerarchia, il numero > 0, il nome del file temporaneo e i singoli caratteri 
FCR.sh $G $L /tmp/nomiAssoluti $*

echo Numero totale di file trovati che soddisfano la specifica `wc -l < /tmp/nomiAssoluti`

#per ogni file valido trovato
for F in `cat /tmp/nomiAssoluti`
do
	echo Invocazione parte C con parametri $F $L $*
done

#rimozione file temporaneo
rm /tmp/nomiAssoluti
