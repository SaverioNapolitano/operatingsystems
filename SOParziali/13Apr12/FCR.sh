#!/bin/sh

#file comandi ricorsivo

cd $1 #mi sposto nella directory corrispondente

conta=`expr $2 + 1` #considero 1 il primo livello della gerarchia
#esploro tutta la gerarchia
for i in *
do
	#leggo il valore memorizzato nel file se sono nella fase A
	if test $4 = A
	then	read prec < $3
		#se minore del livello attuale memorizzato in conta lo sovrascrivo
		if test $prec -lt $conta
		then	echo $conta > $3
		fi
	fi
done

#se sono nella fase B controllo di essere al livello inserito da utente
if test $4 = B
then	if test $conta -eq $3
	then	#stampo informazioni directory esclusi file nascosti
		echo Stampo le informazioni della directory $1
		ls -l $1
	fi
fi

#ricorsione su tutta la gerarchia
for i in *
do
	#se directory e traversabile
	if test -d $i -a -x $i
	then	FCR.sh `pwd`/$i $conta $3 $4 #invocazione file comandi ricorsivo con primo parametro la directory corrispondente e secondo parametro il conteggio dei livelli 
	fi
done
