#!/bin/sh

#file comandi ricorsivo

#mi sposto nella directory corrente passata come parametro
cd $1

#esploro la gerarchia
for F in *
do
	#se e\' file
	if test -f $F 
	then	#se la sua lunghezza e\' non nulla
		if test `wc -l < $F` -gt 0
		then	#memorizzo contenuto file in una variabile 
			contenuto=`cat < $F`
			#controllo che contenga solo caratteri alfabetici minuscoli
			case $contenuto in
			*[!a-z]*)	;; #file non valido
			*)	#file valido, si riporta su standard output il suo nome assoluto
				echo Trovato file valido=`pwd`/$F
				#lo inserisco nel file temporaneo passato come parametro
				echo `pwd`/$F >> $2;;
			esac
		fi
	fi
done

#parte ricorsiva
for G in *
do
	#se directory e traversabile
	if test -d $G -a -x $G
	then	#invocazione con primo parametro la directory corrente (gli altri parametri restano invariati)
		FCR.sh $G $2
	fi
done
