#!/bin/sh

#file comandi ricorsivo

cd $1

for F in *
do
	#se file e leggibile
	if test -f $F -a -r $F
	then	#salvo le sue righe in una variabile
		NR=`wc -l < $F`
		#se sono almeno tante quante il secondo parametro
		if test $NR -ge $2
		then	
			#se sono almeno 5 linee
			if test $NR -ge 5
			then	#salvo la sua quinta linea in un nuovo file
				> /tmp/$F.quinta
				echo `head -5 $F | tail -1 >> /tmp/$F.quinta`
				#salvo il nome nel file temporaneo
				echo /tmp/$F.quinta >> $3
			else	#creo un file vuoto
				> /tmp/$F.NOquinta
				#salvo il nome nel file temporaneo
				echo /tmp/$F.NOquinta >> $3
			fi
		fi
	fi
done

#parte ricorsiva
for G in *
do
	if test -d $G -a -x $G
	then	FCR.sh $G $2 $3
	fi
done
