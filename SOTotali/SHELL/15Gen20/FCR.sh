#!/bin/sh

#file comandi ricorsivo

cd $1 #mi sposto nella directory corrispondente

#esploro tutta la gerarchia
for i in *
do
	#se file
	if test -f $i
	then	#se lunghezza in caratteri uguale al secondo parametro
		if test `wc -c < $i` -eq $2
		then	#file valido, riporto nome assoluto su standard output
			echo Trovato file valido `pwd`/$i
			#salvo nome assoluto su file temporaneo
			echo `pwd`/$i >> $3
		fi
	fi
done

#ricorsione su tutta la gerarchia
for i in *
do
	#se directory e traversabile
	if test -d $i -a -x $i
	then	#invocazione file comandi ricorsivo con primo parametro la directory corrispondente
		FCR.sh $i $2 $3
	fi
done
