#!/bin/sh

#file comandi ricorsivo

cd $1

#variabile che mi dice se directory valida
valid=false

for F in *
do
	#se file 
	if test -f $F
	then	#controllo che abbia la terminazione richiesta
		case $F in
		*.$2)	#directory valida, aggiorno variabile
			valid=true;;
		*)	;;
		esac
	fi
done

#se directory valida
if test $valid = true
then	#scrivo il suo nome sul file temporaneo
	echo $1 >> $3
fi

#parte ricorsiva
for G in *
do
	#se directory e traversabile
	if test -d $G -a -x $G
	then	FCR.sh `pwd`/$G $2 $3
	fi
done
