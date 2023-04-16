#!/bin/sh

#file comandi principale

#controllo che siano passati almeno 3 parametri
case $# in
0|1|2)	echo Errore: necessari almeno 3 parametri, $# sono pochi
	exit 1;;
*)	echo OK passati $# parametri;;
esac

#variabile in cui salvo il primo parametro
X=

#controllo che il primo parametro sia un numero intero > 0
case $1 in
*[!0-9]*)	#se fa match significa che contiene almeno un carattere non numerico
		echo Errore: $1 non numero o non positivo
		exit 2;;
*)		#altrimenti numero, controllo che non sia 0
		if test $1 -eq 0
		then	echo Errore: $1 corrisponde a zero
			exit 3
		fi;;
esac

#se arrivo qui parametro valido, lo salvo
X=$1

#ora posso usare lo shift per avere nella lista dei parametri solo le gerarchie da controllare
shift

#controllo che i parametri siano nomi assoluti directory
for G
do
	case $G in
	/*)	#nome assoluto, controllo sia directory
		if test ! -d $G -o ! -x $G
		then	echo Errore: $G non directory o non traversabile
			exit 4
		fi;;
	*)	#non nome assoluto
		echo Errore: $G non nome assoluto
		exit 5;;
	esac
done

#settaggio ed export del PATH
PATH=`pwd`:$PATH
export PATH

#variabile che memorizza la fase corrente
N=1

for G
do
	echo DEBUG- fase $N
	#creazione file temporaneo
	> /tmp/nomiAssoluti$N
	echo /tmp/nomiAssoluti$N
	#invocazione file comandi ricorsivo passando come parametri la gerarchia corrente, il numero intero > 0 e il file temporaneo corrente
	FCR.sh $G $X  /tmp/nomiAssoluti$N
	echo Gerarchia $G numero di file trovati=`wc -l < /tmp/nomiAssoluti$N`
	#incremento contatore
	N=`expr $N + 1`
done

#riporto il contatore a 2
N=2
#per ogni file trovato nella prima gerarchia
for i in `cat /tmp/nomiAssoluti1`
do
	#per tutte le altre gerarchie
	for j 
	do
		#per ogni file trovato nella gerarchia n-sima
		for F in `cat /tmp/nomiAssoluti$N`
		do
			#controllo se ci sono differenze fra i file ridirezionando standard output (non ridireziono standard error siccome mi serve)
			`diff $i $F > /dev/null`
			#se non ci sono allora sono uguali
			if test $? -eq 0
			then	echo I file $i e $F sono uguali
			fi 
		done
		#incremento il contatore
		N=`expr $N + 1`
		#se contatore maggiore del numero di gerarchie da controllare
		if test $N -gt $#
		then	#esco dal ciclo
			break
		fi
	done
	#ripristino il contatore
	N=2
done

#riporto contatore a 1
N=1

#rimozione file temporanei
#ho creato tanti file temporanei quanti sono i parametri, quindi posso scorrere la lista dei parametri per determinare quante iterazioni fare
for i 
do
	rm /tmp/nomiAssoluti$N
	#incremento il contatore
	N=`expr $N + 1`
done
