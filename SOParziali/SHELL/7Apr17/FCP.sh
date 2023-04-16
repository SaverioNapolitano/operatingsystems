#!/bin/sh

#file comandi principale

#controllo che siano passati almeno 3 parametri
case $# in
0|1|2)	echo Errore: necessari almeno 3 parametri, $# sono pochi
	exit 1;;
*)	echo OK passati $# parametri;;
esac

#controllo che primo parametro sia nome relativo semplice file
case $1 in
*/*)	#se contiene / non nome relativo semplice
	echo Errore: $1 deve essere nome relativo semplice
	exit 2;;
*)	;; #nome relativo semplice
esac

#se arrivo qui primo parametro valido, lo salvo in una variabile e faccio lo shift
F=$1
shift

#controllo che i restanti N parametri siano nomi assoluti directory
for G 
do
	case $G in
	/*)	#nome assoluto, controllo sia directory
		if test ! -d $G -o ! -x $G
		then	echo Errore: $G non directory o non traversabile
			exit 4
		fi;;
	*)	#non nome assoluto
		echo Errore: $G non nome assoluto
		exit 5;;
	esac
done

#controlli finiti, ora settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

#creazione file temporaneo
> /tmp/conta$$

#passo ora alle N fasi
for G
do
	#invocazione file comandi ricorsivo passando come parametri la gerarchia corrispondente, il nome relativo semplice del file e il file temporaneo
	FCR.sh $G $F /tmp/conta$$
done

echo Numero totale di file creati globalmente=`wc -l < /tmp/conta$$`

#per ogni file creato
for F in `cat /tmp/conta$$`
do
	echo Nome assoluto del file=$F
	echo Prima riga del file 
	head -1 $F
	echo Ultima riga del file
	tail -1 $F
done

#rimozione file temporaneo
rm /tmp/conta$$
