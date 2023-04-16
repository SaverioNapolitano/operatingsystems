#!/bin/sh

#file comandi principale

#controllo che il numero dei parametri passati sia almeno 3
case $# in
0|1|2)	echo Errore: necessari almeno 3 parametri, $# sono pochi
	exit 1;;
*)	echo OK passati $# parametri;;
esac

#controllo che il primo parametro sia un numero intero strettamente positivo
case $1 in
*[!0-9]*)	echo Errore: $1 non numero o non positivo
		exit 2;;
*)		#se arrivo qui e\' un numero positivo, controllo che non sia 0
		if test $1 -eq 0
		then	echo Errore: $1 e\' zero
			exit 3
		fi;;
esac

#se arrivo qui significa che il numero e\' valido, lo memorizzo in una variabile
Y=$1

#con lo shift la lista dei parametri contiene solo le gerarchie da controllare
shift

#posso scorrere tutta la lista dei parametri con un for
for G
do
	case $G in
	/*)	#nome assoluto, controllo che sia directory
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

#creazione file temporaneo
>/tmp/conta$$

#invocazione ricorsiva
for G
do
	FCR.sh $G $Y /tmp/conta$$
done

#memorizzo il numero di file creati globalmente in una variabile
NF=`wc -l < /tmp/conta$$`

echo Globalmente sono stati creati $NF file

#contatore che mi dice il numero del file 
N=1

for F in `cat < /tmp/conta$$`
do
	echo Nome assoluto del file $N=$F
	echo Adesso mostro il suo contenuto, quinta riga dall\' inizio del file da cui e\' stato creato
	cat $F
done

#rimozione file temporaneo
rm /tmp/conta$$
