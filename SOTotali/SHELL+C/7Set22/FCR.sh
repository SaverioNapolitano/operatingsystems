#!/bin/sh

#file comandi ricorsivo

#mi sposto nella directory corrente passata come parametro
cd $1
#variabile per memorizzare la posizione del file/directory nella gerarchia
pos=0
#variabile per memorizzare numero di file trovati
num=0
#variabile per memorizzare nome file trovati
files=

#esploro gerarchia
for F in *
do
	#incremento la posizione
	pos=`expr $pos + 1`
	#se e\' file ed e\' leggibile
	if test -f $F -a -r $F
	#se la lunghezza in caratteri del file e\' uguale al numero passato come parametro in $2
	then	
		if test `wc -c < $F` -eq $2
		then	#se la posizione e\' dispari
			if test `expr $pos % 2` -ne 0
			then	echo Trovato file valido=`pwd`/$F
				#incremento il contatore
				num=`expr $num + 1`
				#salvo il nome del file
				files="$files $F"
			fi
		fi
	fi
done

#se trovato almeno un file
if test $num -ge 1
then	echo Invocazione parte C con parametri $files
fi

#parte ricorsiva
for G in *
do
	#se directory e traversabile
	if test -d $G -a -x $G
	then	#invocazione file comandi ricorsivo passando come primo parametro la directory corrente (gli altri parametri rimangono invariati)
		FCR.sh `pwd`/$G $2
	fi
done
