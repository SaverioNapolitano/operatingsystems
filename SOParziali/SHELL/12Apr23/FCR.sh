#!/bin/sh

#file comandi ricorsivo

cd $1 #mi sposto nella directory corrispondente

#esploro la gerarchia
for file in *
do
	#se e\' file ed e\' leggibile
	if test -f $file -a -r $file
	then	#controllo la sua estensione
		case $file in
		*.$2)	#file ha come estensione la prima stringa, lo salvo sul corrispondente file temporaneo
			echo `pwd`/$file >> $4;;
		*.$3)	#file ha come estensione la seconda stringa, lo salvo sul corrispondente file temporaneo
			echo `pwd`/$file >> $5;;
		esac #se il file non ha una estensione che mi interessa lo ignoro e vado avanti
	fi
done

#ricorsione su tutta la gerarchia
for dirAss in *
do
	#se e\' directory ed e\' traversabile
	if test -d $dirAss -a -x $dirAss
	then	#invocazione file comandi ricorsivo passando come primo parametro la directory corrente, il secondo e terzo parametro sono le due estensioni da controllare, il quarto e quinto parametro sono i nomi assoluti dei file temporanei
		FCR.sh `pwd`/$dirAss $2 $3 $4 $5
	fi
done
