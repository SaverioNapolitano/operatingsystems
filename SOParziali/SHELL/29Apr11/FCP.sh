#!/bin/sh

#file comandi principale

#controllo che siano passatti esattamente tre parametri
case $# in
3)	echo OK passati 3 parametri;;
*)	echo Errore: necessari 3 parametri, non $#
	exit 1;;
esac

#controllo che primo parametro sia nome assoluto directory
case $1 in
/*)	#nome assoluto, controllo sia directory
	if test ! -d $1 -o ! -x $1
	then	echo Errore: $1 non directory o non traversabile
		exit 2
	fi;;
*)	#non nome assoluto
	echo Errore: $1 non nome assoluto
	exit 3;;
esac

#controllo che secondo parametro sia nome relativo semplice file
case $2 in
*/*)	#se contiene / in qualunque posizione non nome relativo semplice
	echo Errore: $2 non nome relativo semplice
	exit 4;;
*)	;; 
esac

#controllo che terzo parametro sia numero intero positivo
case $3 in
*[!0-9]*)	#se contiene in qualunque posizione carattere non numerico allora non numero o non positivo
		echo Errore: $3 non numero o non positivo
		exit 5;;
*)		;;
esac

#controlli finiti, settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

#variabile per memorizzare la fase corrente (inizialmente posta uguale alla prima fase A)
fase=A

#creazione file temporaneo
> /tmp/conta$$

#passo ora alla fase A, invocando il file comandi ricorsivo con parametri la gerarchia corrispondende, il nome relativo semplice del file, il nome del file temporaneo e la fase
FCR.sh $1 $2 /tmp/conta$$ $fase

echo Numero di file che soddisfano le specifiche=`wc -l < /tmp/conta$$`

#se numero file trovati minore del numero passato come terzo parametro
if test `wc -l < /tmp/conta$$` -lt $3
then	#passo alla fase B
	fase=B
	#invoco file comandi ricorsivo con stessi parametri
	FCR.sh $1 $2 /tmp/conta$$ $fase
fi

#se ho eseguito fase B allora devo eseguire fase C (uguale alla fase A)
if test $fase = B
then	fase=A
	#azzero file temporaneo
	> /tmp/conta$$
	#invocazione file comandi ricorsivo come per fase A
	FCR.sh $1 $2 /tmp/conta$$ $fase
	echo Numero di file che soddisfano le specifiche=`wc -l < /tmp/conta$$`
fi
	
#rimozione file temporaneo
rm /tmp/conta$$
