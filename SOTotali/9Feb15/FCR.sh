#!/bin/sh

#file comandi FCR.sh

#mi sposto nella directory corrente

cd $1

#variabile per contare i file trovati
cont=0

#esploro la directory corrente
for i in *
do
	if test -f $i -a -r $i #se e\' file ed e\' leggibile
	then	echo DEBUG-- $i file leggibile
		if test `wc -l < $i` -eq $2 #se il numero di linee del file e\' uguale al parametro passato
		then	echo DEBUG-- $i ha $2 linee
			cont=`expr $cont + 1` #incremento il contatore
			echo `pwd`/$i >> $3 #memorizzo nome assoluto in file temporaneo (passato come parametro)
		fi
	fi
done

if test $cont -gt 0 #se trovato almeno un file
then	echo Directory valida $1 #stampo su standard output la directory corrente
fi

#parte ricorsiva
for i in *
do
	if test -d $i -a -x $i #se e\' directory ed e\' traversabile
	then	echo DEBUG-- $i directory traversabile
		echo DEBUG-- Ricorsione su $i
		FCR.sh `pwd`/$i $2 $3 #chiamo file ricorsivo con nome assoluto directory corrente come primo parametro (gli altri sono invariati)
	fi
done
