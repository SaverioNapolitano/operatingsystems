#!/bin/sh

#file comandi ricorsivo

cd $1 #mi sposto nella directory corrispondente

#devo controllare anche la radice della gerarchia
#controllo che esista un file con nome relativo semplice $2
if test -f $2
then	if test $4 = A #se fase A
	then	#directory valida, riporto nome assoluto su standard output
		echo Nome assoluto directory valida per fase A o C
		pwd
	fi
else	#se non esiste file con nome valido
	if test $4 = B #se fase B
	then	#directory valida, riporto nome assoluto su standard output
		echo Nome assoluto directory valida per fase B
		pwd
		#creazione file vuoto con nome relativo semplice $2
		> $2
	fi
fi

#esploro tutta la gerarchia
for i in *
do
	#se file e fase A
	if test -f $i -a $4 = A
	then	#controllo che il suo nome relativo semplice sia quello giusto 
		case $i in
		$2)	#file valido, salvo nome assoluto su file temporaneo
			echo `pwd`/$i >> $3;;
		*)	;; #file non valido
		esac
	fi
done

#parte ricorsiva
for i in *
do
	#se directory e traversabile
	if test -d $i -a -x $i
	then	#invocazione file ricorsivo con primo parametro la directory corrispondente
		FCR.sh $i $2 $3 $4
	fi
done
