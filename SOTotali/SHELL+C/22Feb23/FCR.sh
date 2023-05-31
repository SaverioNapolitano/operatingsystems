#!/bin/sh

#file comandi ricorsivo

#mi sposto nella directory corrente passata come parametro
cd $1

#esploro la gerarchia
for F in *
do
	#se e\' file ed e\' leggibile
	if test -f $F -a -r $F
	then	#se la loro lunghezza in caratteri e\' minore del numero passato come parametro $2
		if test `wc -c < $F` -lt $2
		then	#salvo il nome assoluto sul file temporaneo passato come parametro $3
			echo `pwd`/$F >> $3
			#e lo riporto su standard output
			echo Trovato file valido=`pwd`/$F
		fi
	fi
done

#parte ricorsiva
for G in *
do
	if test -d $G -a -x $G
	then	FCR.sh `pwd`/$G $2 $3
	fi
done
