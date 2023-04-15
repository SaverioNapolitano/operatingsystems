#!/bin/sh

#file comandi principale

#controllo che sia passato un solo parametro e che sia nome assoluto directory
case $# in
1)	echo OK passato un parametro
	case $1 in
	/*)	#se inizia per / nome assoluto, controllo sia dieectory
		if test ! -d $1 -o ! -x $1
		then	echo Errore: $1 non directory o non traversabile
			exit 1
		fi;;
	*)	#altrimenti non nome assoluto
		echo Errore: $1 non nome assoluto
		exit 2;;
	esac;;
*)	echo Errore: necessario esattamente un parametro, non $#
	exit 3;;
esac

#controlli sui parametri finiti, settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

#creazione file temporaneo
> /tmp/contaLivelli

#variabile che conta i livelli
conta=0

#inserisco conta ovvero 0 nel file temporaneo
echo $conta > /tmp/contaLivelli

#fase A: invocazione file comandi ricorsivo con parametri la gerarchia, la variabile per contare i livelli, il file temporaneo e un carattere che indica in che fase sono
FCR.sh $1 $conta /tmp/contaLivelli A

#leggo dal file quanti livelli ha la gerarchia
read livelli < /tmp/contaLivelli
echo Numero livelli totale gerarchia $livelli

#fase B
echo Inserire numero pari fra 1 e $livelli
read num

#controllo che il numero inserito sia pari e compreso fra 1 e i livelli della gerarchia
case $num in
*[!0-9]*)	#se contiene in qualunque posizione carattere non numerico 
		echo Errore: $num non numero o non positivo
		exit 4;;
*)		#altrimenti numero, controllo sia fra 1 e i livelli della gerarchia
		if test $num -lt 1 -o $num -gt $livelli
		then	echo Errore: $num non compreso fra 1 e $livelli
			exit 5
		fi
		#se compreso fra 1 e livello gerarchia controllo sia pari
		if test `expr $num % 2` -ne 0
		then	#se il resto della divisione per 2 diverso da 0 allora non pari
			echo Errore: $num non pari
			exit 6
		fi;;
esac

#se arrivo qui numero valido, invoco file comandi ricorsivo con secondo parametro il livello scelto da utente
FCR.sh $1 $conta $num B

#rimozione file temporaneo
rm /tmp/contaLivelli

