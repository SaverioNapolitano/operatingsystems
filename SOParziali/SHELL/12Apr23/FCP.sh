#!/bin/sh

#file comandi principale

#controllo che siano passati almeno 4 parametri
case $# in
0|1|2|3)	echo Errore: necessari almeno 4 parametri, $# sono pochi
		exit 1;;
*)		echo OK passati $# parametri;;
esac

#controllo che il primo parametro sia una stringa semplice
case $1 in
*/*)	#se contiene uno / in qualunque posizione la stringa non e\' valida
	echo Errore: $1 non deve contenere /
	exit 2;;
*)	;; #altrimenti la stringa va bene
esac

#se arrivo qui il primo parametro e\' valido, lo salvo in una variabile
str1=$1

#faccio lo stesso controllo per il secondo parametro
case $2 in
*/*)	#se contiene uno / in qualunque posizione la stringa non e\' valida 
	echo Errore: $2 non deve contenere /
	exit 3;;
*)	;; #altrimenti la stringa va bene
esac

#se arrivo qui il secondo parametro e\' valido, lo salvo in una variabile
str2=$2

#ora posso fare due shift
shift
shift

#ora la lista dei parametri contiene solo le Z gerarchie da controllare
#posso scorrerla con un for
for dirAss
do
	#controllo che sia nome assoluto directory
	case $dirAss in
	/*)	#se inizia con / nome assoluto, controllo che sia directory
		if test ! -d $dirAss -o ! -x $dirAss
		then	echo Errore: $dirAss non directory o non traversabile
			exit 4
		fi;;
	*)	#altrimenti non nome assoluto
		echo Errore: $dirAss non nome assoluto
		exit 5;;
	esac
done

#controlli finiti, settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

#variabile che contiene il numero totale di file trovati con estensione str1
SOMMA1=0

#variabile che contiene il numero totale di file trovati con estensione str2
SOMMA2=0

#creazione file temporaneo che contiene nomi file con estensione str1
> /tmp/absoluteDirs1$$

#creazione file temporaneo che contiene nomi file con estensione str2
> /tmp/absoluteDirs2$$

#passo alle Z fasi
for dirAss
do
	#invocazione file comandi ricorsivo con parametri la gerarchia corrente, le due stringhe semplici e i file temporanei
	FCR.sh $dirAss $str1 $str2 /tmp/absoluteDirs1$$ /tmp/absoluteDirs2$$ 
done

#aggiorno i valori di SOMMA1 e SOMMA2 contando le linee dei rispettivi file
SOMMA1=`wc -l < /tmp/absoluteDirs1$$`
SOMMA2=`wc -l < /tmp/absoluteDirs2$$`

echo Numero totale di file trovati con estensione $str1=$SOMMA1
echo Numero totale di file trovati con estensione $str2=$SOMMA2

#se SOMMA2 minore o uguale di SOMMA1
if test $SOMMA2 -le $SOMMA1
then	#chiedo a utente un numero intero compreso fra 1 e SOMMA2
	echo $USER inserisci un numero intero compreso fra 1 e $SOMMA2
	read N
	#variabile che mi dice se il numero va bene
	valido=true
	#controllo che il numero inserito sia corretto
	case $N in
	*[!0-9]*)	#se contiene in qualunque posizione un carattere non numerico
			echo $N non numero o non positivo, non faccio nulla
			valido=false;;
	*)		#se numero positivo controllo che sia fra 1 e SOMMA2
			if test $N -lt 1 -o $N -gt $SOMMA2
			then	echo $N non compreso fra 1 e $SOMMA2, non faccio nulla
				valido=false
			fi;;
	esac
	#se valido ha mantenuto il suo valore iniziale allora il numero va bene
	if test $valido = true
	then	#riporto su stdout nome assoluto di N-simo file con estensione str1 ed N-simo file con estensione str2
		echo file $N con terminazione $str1
		#prendo le prime N linee del file, poi di queste N linee tengo solo l'ultima
		head -$N /tmp/absoluteDirs1$$ | tail -1
		echo file $N con terminazione $str2
		#prendo le prime N linee del file, poi di queste N linee tengo solo l'ultima
		head -$N /tmp/absoluteDirs2$$ | tail -1
	fi
else	#se SOMMA2 maggiore di SOMMA1 non devo fare niente
	echo Siccome $SOMMA2 maggiore di $SOMMA1 non faccio nulla
fi

#rimozione file temporanei
rm /tmp/absoluteDirs1$$
rm /tmp/absoluteDirs2$$

