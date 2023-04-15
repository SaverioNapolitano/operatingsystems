#!/bin/sh

case $# in
4)	echo Il numero di parametri passati e corretto;;
*)	echo Il numero di parametri passati e sbagliato
	exit 1;;
esac

G= 
Cx=

case $1 in 
/*)	echo Il percorso e assoluto
	if test ! -d $1 -o ! -x $1
	then	echo non dir o non trav
		exit 3
	fi;;
*)	echo Il percorso non e assoluto 
	exit 2;;
esac
G=$1

case $4 in
?)	echo ultimo parametro e singolo carattere
	Cx=$4;;
*)	echo ultimo parametro non e singolo carattere
	exit 4;;
esac
set $2 $3

for i 
do
	case $i in
	*[!0-9]*)	echo non numero
			exit 5;;
	*)		if test $i -eq 0
			then	echo numero non strettamente positivo
				exit 6
			fi;;
	esac
done

PATH=`pwd`:$PATH
export PATH

FCR.sh $G $1 $2 $Cx

