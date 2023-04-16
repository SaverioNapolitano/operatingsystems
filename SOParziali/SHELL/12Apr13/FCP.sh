#!/bin/sh

#file comandi principale

#controllo che siano passati esattamente tre parametri
case $# in
0|1|2)	echo Errore: servono 3 parametri, $# sono pochi
	exit 1;;
3)	echo OK, passati 3 parametri;;
*)	echo Errore: necessari 3 parametri, $# sono troppi
	exit 2;;
esac

#controllo che primo parametro sia nome assoluto directory
case $1 in
/*)	#se inizia per / nome assoluto, controllo sia directory
	if test ! -d $1 -o ! -x $1
	then	echo Errore: $1 non directory o non traversabile
		exit 3
	fi;;
*)	#se non inizia per / non nome assoluto
	echo Errore: $1 non nome assoluto
	exit 4;;
esac

#controllo che il secondo parametro non contenga /
case $2 in
*/*)	#se contiene / in una qualunque posizione stringa non valida
	echo Errore: $2 non deve contenere /
	exit 5;;
*)	#se non contiene / in nessuna posizione stringa valida
	;;
esac

#controllo che terzo parametro sia numero intero strettamente positivo
case $3 in
*[!0-9]*)	#se contiene in una qualunque posizione un carattere non numerico allora non numero o non positivo
		echo Errore: $3 non numero o non positivo
		exit 6;;
*)		#se contiene solo caratteri numerici allora numero positivo, controllo non sia 0
		if test $3 -eq 0
		then	echo Errore: $3 non deve essere zero
			exit 7
		fi;;
esac

#controlli finiti, ora settaggio ed export PATH
PATH=`pwd`:$PATH
export PATH

#creazione file temporaneo
> /tmp/conta$$

#passo ora alla fase A invocando file comandi ricorsivo con parametri directory $1, stringa $2 e file temporaneo
echo Fase A
FCR.sh $1 $2 /tmp/conta$$

echo Numero totale di directory della gerarchia che soddisfano le specifiche `wc -l < /tmp/conta$$`
echo Nomi assoluti delle directory trovate
cat /tmp/conta$$

#controllo se numero directory trovate strettamente maggiore del terzo parametro $3
if test `wc -l < /tmp/conta$$` -gt $3
then	#se maggiore passo alla fase B
	echo Fase B
	echo Inserire un numero fra 1 e $3
	read num
	#controllo che numero inserito sia valido (stesso controllo del terzo parametro)
	case $num in
	*[!0-9]*)	echo Errore: $num non numero o non positivo
			exit 8;;
	*)		if test $num -eq 0
			then	echo Errore: $num deve essere compreso fra 1 e $3
				exit 9
			fi;;
	esac
	#salvo il nome assoluto in una variabile
	DIR=`head -$num /tmp/conta$$ | tail -1`
	echo Nome assoluto della directory $num $DIR
	#invoco nuovamente il file ricorsivo con il terzo parametro che invece di essere il nome assoluto di un file sara\' una stringa semplice
	stampa=stampa
	FCR.sh $DIR $2 $stampa
fi

#rimozione file temporaneo
rm /tmp/conta$$
