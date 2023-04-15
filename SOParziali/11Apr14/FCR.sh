#!/bin/sh

#file comandi ricorsivo

cd $1 #mi sposto nella directory giusta

#esploro la gerarchia
for i in *
do
	if test -f $i -a -r $i #se file e leggibile
	then	#se tutte le sue linee iniziano per a
		#grep scrive in output tutte le righe che fanno match, le conto e controllo che siano uguali in numero alle linee totali del file (che devono essere maggiori di 0)
		if test `grep '^a' < $i | wc -l` -eq `wc -l < $i` -a `wc -l < $i` -ne 0
		then	#file valido, salvo il suo nome assoluto sul file temporaneo
			echo `pwd`/$i >> $2
		fi
	fi
done

#esplorazione ricorsiva della gerarchia
for i in *
do
	if test -d $i -a -x $i #se directory e traversabile
	then	FCR.sh `pwd`/$i $2 #invocazione file comandi ricorsivo con primo parametro la directory corrispondente
	fi
done
