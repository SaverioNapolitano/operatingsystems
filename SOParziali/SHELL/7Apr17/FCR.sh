#!/bin/sh

#file comandi ricorsivo

#mi sposto nella directory corrente
cd $1

#esploro la gerarchia
for F in *
do
	#se file e leggibile
	if test -f $F -a -r $F
	then	#controllo se il suo nome coincida col secondo parametro
		case $F in
		$2)	#nome valido, creo file sorted
			sort -f `pwd`/$F > sorted
			#salvo nome assoluto di sorted su file temporaneo
			echo Nome assoluto file `pwd`/sorted
			echo `pwd`/sorted >> $3;;
		*)	;; #nome non valido
		esac
	fi
done

#parte ricorsiva
for G in *
do
	#se directory e traversabile
	if test -d $G -a -x $G
	then	#invocazione file comandi ricorsivo con stessi parametri $2 e $3 e come primo parametro la directory corrente
		FCR.sh `pwd`/$G $2 $3
	fi
done
