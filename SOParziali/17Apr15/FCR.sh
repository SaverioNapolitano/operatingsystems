#!/bin/sh

#file comandi ricorsivo

#mi sposto nella directory passata come parametro
cd $1

#esploro la gerarchia
for i in *
do
	#se e\' file ed e\' leggibile
	if test -f $i -a -r $i
		#se il numero di linee del file che terminano con il carattere t e\' uguale al parametro passato
	then	if test `grep 't$' < $i | wc -l` -ge $2
		#salvo il nome assoluto sul file temporaneo
		then echo `pwd`/$i >> $3
		fi
	fi
done

#parte ricorsiva
for i in *
do
	#se directory traversabile
	if test -d $i -a -x $i
	#ricorsione con primo parametro nome assoluto directory corrente (gli altri parametri restano invariati)
	then	FCR.sh `pwd`/$i $2 $3
	fi
done
