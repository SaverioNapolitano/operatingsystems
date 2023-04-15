#!/bin/sh

#file comandi ricorsivo

cd $1 #mi sposto nella directory corrispondente

L=$2
TMP=$3

shift 3

#esplorazione della gerarchia
for i in *
do
	#se file 
	if test -f $i
	then	#se lunghezza in linea uguale al secondo parametro
		if test `wc -l < $i` -eq $L
		then	#variabile che mi dice se il file e\' valido
			valido=true
			#controllo che abbia almeno una occorrenza di tutti i caratteri
			for j
			do
				case $j in
				?)	#se singolo carattere ha senso controllare che compaia nel file
					if grep $j $i > /dev/null 2>&1 #ridirezione su stdout e stderr
					then	#passo alla iterazione successiva
						continue
					else	#altrimenti se il carattere non compare il file non e\' valido
						valido=false
						break
					fi;;
				*)	#se non e\' singolo carattere vado avanti
					continue;;
				esac
			done
			#se valido ha conservato valore iniziale allora il file rispetta le specifiche
			if test $valido = true
			then	#stampo il suo nome assoluto su stdout
				echo Trovato file valido `pwd`/$i
				#e lo salvo su file temporaneo
				echo `pwd`/$i >> $TMP
			fi
		fi
	fi
done

#ricorsione su tutta la gerarchia
for i in *
do
	#se directory e traversabile
	if test -d $i -a -x $i
	then	#invocazione file comandi ricorsivo con primo parametro la directory corrispondente
		FCR.sh `pwd`/$i $L $TMP $*
	fi
done
