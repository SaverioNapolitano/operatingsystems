#!/bin/sh

#file comandi ricorsivo

cd $1

for i in *
do
	#se file e scrivibile e leggibile
	if test -f $i -a -r $i -a -w $i
	then	#controllo che il suo nome sia valido
		echo Possibile file valido $i
		case $i in
		$2.txt) #salvo nome assoluto su file temporaneo
			echo File TROVATO $i
			echo `pwd`/$i >> $3
			#salvo lunghezza in caratteri su file temporaneo
			echo `wc -c < $i` >> $3;;
		*)	echo File non valido;;
		esac
	fi
done

#parte ricorsiva
for i in *
do
	#se directory e traversabile
	if test -d $i -a -x $i
	then	FCR.sh $i $2 $3
	fi
done
