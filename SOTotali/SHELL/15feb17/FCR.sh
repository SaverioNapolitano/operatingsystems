#!/bin/sh

#file comandi FCR.sh

#mi sposto nella gerarchia da esplorare
cd $1

valido=true #variabile che mi dice se file valido
file= #variabile per memorizzare nomi file trovati
linea= #variabile per scorrere le linee del file
cont=0 #variabile per contare file trovati
c=1 #variabile per contare le linee del file dall'inizio
#esploro gerarchia
for i in *
do
	#se file
	if test -f $i
	#se leggibile e numero di linee uguali al parametro passato 
	then	if test -r $i -a `wc -l < $i` -eq $2
		then	echo DEBUG-- valore di i=$i
			for j in `cat $i` #TODO: trovare scorrimento
			do	#echo DEBUG-- valore di j $j
				linea=`head -$c < $i | tail -1` #assegno a linea le prime c linee del file che voglio esaminare
				echo DEBUG-- valore di linea=$linea
				case $linea in
				*[0-9]*)echo DEBUG-- carattere numerico trovato;; #se trovo carattere numerico ok
				*)	echo DEBUG-- carattere numerico non trovato
					valido=false #altrimenti setto valido a false
					break;; #esco dal ciclo
				esac
				if test $c -eq $2 #se ho scorso tutte le linee esco
				then	break
				fi
				c=`expr $c + 1` #incremento numero di linee scorse 
			done
			c=1 #riporto a 1 il numero di linea per scorrere il prossimo file
			echo DEBUG-- valore di valido=$valido
			if test $valido = true #se file valido
			then	file="$file $i" #salvo nome file
				cont=`expr $cont + 1` #incremento contatore
			fi
			valido=true #ripristino il valore di valido per il file successivo
		fi
	fi
done

if test $cont -ne 0 #se trovato almeno un file
then	echo Directory valida $1 #stampo nome assoluto directory
	echo file validi trovati $file #stampo lista file validi
fi

#parte ricorsiva
for i in *
do
	if test -d $i
	then	FCR.sh `pwd`/$i $2
	fi
done
