#!/bin/sh

#file comandi ricorsivo

cd $1

for i in *
do
	#se directory e traversabile
	if test -d $i -a -x $i
	then	case $i in
		$2?$2)	#se nome di tre caratteri e i dispari sono uguali al secondo parametro
			#salvo il nome nel file temporaneo
			echo `pwd`/$i >> $3;;
		*)	;;
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
