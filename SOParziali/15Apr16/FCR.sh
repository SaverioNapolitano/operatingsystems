#!/bin/sh

#file comandi ricorsivo

#mi sposto nella directory giusta
cd $1

#variabile che mi dice se directory valida
valida=true

#esploro la gerarchia
for F in *
do
	#se non file
	if test ! -f $F
	then 	#directory non valida
		valida=false
	else	#se file controllo sua lunghezza in linee sia maggiore del numero passato come secondo parametro
		if test `wc -l < $F` -le $2
		then	#se lunghezza minore o uguale directory non valida
			valida=false
		fi
	fi
done

#se directory valida salvo suo nome nel file temporaneo
if test $valida = true
then	pwd >> $3 #non serve echo siccome pwd stampa percorso assoluto directory corrente
fi

#parte ricorsiva
for D in *
do
	#se directory e traversabile
	if test -d $D -a -x $D
	then	#invocazione file comandi ricorsivo con parametri invariati $2 e $3 e come primo parametro nome assoluto directory corrente
		FCR.sh `pwd`/$D $2 $3
	fi
done
