#!/bin/sh

#file comandi ricorsivo

#mi sposto nella gerarchia corrente
cd $1

#esploro la gerarchia corrente
for F in *
do
	#se file e leggibile
	if test -f $F -a -r $F
	then	#se lunghezza pari al secondo parametro
		if test `wc -l < $F` -eq $2
		then	#file valido, lo salvo sul file temporaneo
			echo `pwd`/$F >> $3
		fi
	fi
done

#parte ricorsiva
for G in *
do
	#se directory e traversabile
	if test -d $G -a -x $G
	then	#invocazione file comandi ricorsivo con stessi parametri $2 e $3 e directory corrente
		FCR.sh `pwd`/$G $2 $3
	fi
done
