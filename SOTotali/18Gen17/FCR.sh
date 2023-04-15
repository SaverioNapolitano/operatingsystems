#!/bin/sh

#file comandi ricorsivo

cd $1 #mi sposto nella directory corrispondente

#variabile che mi dice se ho stampato directory
stampa=0
#esploro la gerarchia
for D in *
do
	#se directory e traversabile
	if test -d $D -a -x $D
	then	#se ha nome relativo semplice uguale al secondo parametro passato
		if test $D = $2
		then	#mi sposto nella directory corrispondente
			cd `pwd`/$D
			#controllo se ha file che contengono caratteri numerici e li salvo sul file temporaneo
			for j in *
			do
				#se file e leggibile (siccome dopo devo usare grep)
				if test -f $j -a -r $j
				then	#conto quante linee contengono carattere numerico, se almeno una file valido
					if test `grep [0-9] < $j | wc -l` -gt 0
					then	#file valido, salvo nome su file temporaneo
						echo `pwd`/$j >> $3
						#controllo se ho stampato gia nome directory
						if test $stampa -eq 0
						then	#non ho mai stampato nome assoluto directory
							echo Nome assoluto directory valida
							pwd
							#segno che ho stampato
							stampa=1
						fi 
					fi
				fi
			done
		fi
	fi
	#mi riposiziono nella directory passata come primo parametro
	cd $1
done

#parte ricorsiva
for i in *
do
	#se directory e traversabile
	if test -d $i -a -x $i
	then	#invocazione file comandi ricorsivo con primo parametro la directory corrispondente
		FCR.sh `pwd`/$i $2 $3
	fi
done
