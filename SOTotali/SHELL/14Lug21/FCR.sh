#!/bin/sh

#file comandi ricorsivo

#mi sposto nella directory corrente
cd $1

#variabile per contare i file trovati
file=0

#esploro la gerarchia
for i in *
do
	#se file
	if test -f $i
	then	#controllo che il nome abbia esattamente due caratteri
		case $i in
		??)	#? fa match con qualsiasi carattere (ma solo 1)
			#controllo che abbia tante linee quante il parametro passato $3 (M)
			if test `wc -l < $i` -eq $3
			then	#file valido, incremento contatore
				file=`expr $file + 1`
			fi;;
		*)	#in qualsiasi altro caso il nome non ha 2 caratteri
			;;
		esac
	fi
done

#se ho trovato almeno due file ma strettamente meno del numero passato come secondo parametro (H)
if test $file -ge 2 -a $file -lt $2
then	#Directory valida, la stampo
	echo Directory valida=$1
fi

#parte ricorsiva
for i in *
do
	#se directory traversabile
	if test -d $i -a -x $i
	#invoco file comandi ricorsivo passando come primo parametro nome assoluto directory corrente (gli altri parametri sono invariati)	
	then	echo DEBUG-- `pwd`/$i
		FCR.sh `pwd`/$i $2 $3
	fi
done
