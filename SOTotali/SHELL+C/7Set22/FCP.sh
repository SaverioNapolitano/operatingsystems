#!/bin/sh

#file comandi principale

#controllo che siano passati almeno 3 parametri
case $# in
0|1|2)	echo Errore: necessari almeno 3 parametri, $# sono pochi
	exit 1;;
*)	echo OK passati $# parametri;;
esac

#controllo che il primo parametro sia un numero intero > 0 e dispari
case $1 in
*[!0-9]*)	echo Errore: $1 non numero o non positivo
		exit 2;;
*)		#se arrivo qui so che e\' numero, controllo sia dispari e > 0
		if test $1 -eq 0 -o `expr $1 % 2` -eq 0
		then	echo Errore: $1 e\' zero oppure e\' pari
			exit 3
		fi;;
esac

#memorizzo primo parametro in una variabile
X=$1

#faccio uno shift per avere nella lista dei parametri solo le gerarchie da controllare
shift 

#posso scorrerle con un for
for G
do
	#controllo che sia nome assoluto directory
	case $G in
	/*)	#nome assoluto, ora devo controllare che sia directory
		if test ! -d $G -o ! -x $G
		then	echo Errore: $G non directory o non traversabile
			exit 4
		fi;;
	*)	#non nome assoluto
		echo Errore: $G non nome assoluto
		exit 5;;
	esac
done

#settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

#invocazione file comandi ricorsivo
for G 
do
	FCR.sh $G $X
done
