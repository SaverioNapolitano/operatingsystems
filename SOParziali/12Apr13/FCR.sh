#!/bin/sh

#file comandi principale

cd $1 #mi sposto nella directory giusta
#variabile che mi dice se directory valida
valida=false

#esploro tutta la gerarchia
for i in *
do
	#se file e leggibile
	if test -f $i -a -r $i
	then	#controllo che il file abbia terminazione giusta
		case $i in
		*.$2)	#il file ha come terminazione il secondo parametro passato
			#directory valida, se terzo parametro file temporaneo allora quindi metto valida a true
			case $3 in
			/*)	#nome assoluto quindi file temporaneo
				valida=true;;
			*)	#altrimenti significa che devo stampare la prima linea del file trovato e sono nella fase B	
				echo Prima linea del file `pwd`/$i
				head -1 `pwd`/$i;;
			esac;;
		*)	;; #file non valido
		esac
	fi
done

#se valida ha cambiato valore allora directory valida e terzo parametro file temporaneo
if test $valida = true
then	#sono nella fase A e scrivo nome assoluto directory su file temporaneo
	pwd >> $3
fi

#parte ricorsiva
for D in *
do
	#se directory e traversabile
	if test -d $D -a -x $D
	then	#invocazione file comandi ricorsivo passando come primo parametro la directory corrispondente
		FCR.sh $D $2 $3
	fi
done
