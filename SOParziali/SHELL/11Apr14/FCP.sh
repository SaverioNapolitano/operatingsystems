#!/bin/sh

#file comandi principale

#controllo che siano passati almeno 2 parametri
case $# in
0|1)	echo Errore: necessari almeno 2 parametri, $# sono pochi
	exit 1;;
*)	echo OK passati $# parametri;;
esac

#controllo che siano nomi assoluti directory
for i
do
	case $i in
	/*)	#nome assoluto, controllo sia directory
		if test ! -d $i -o ! -x $i
		then	echo Errore: $i non directory o non traversabile
			exit 2
		fi;;
	*)	#non nome assoluto
		echo Errore: $i non nome assoluto
		exit 3;;
	esac
done

#controlli finiti, settaggio ed export PATH
PATH=`pwd`:$PATH
export PATH

#creazione file temporanei
> /tmp/conta$$ #contiene numero di file trovati

#contatore che mi dice la fase corrente
N=1

#passo ora alle N fasi
for i
do
	#creo il file temporaneo dove salvo i nomi assoluti dei file trovati
	> /tmp/files$$-$N
	#invocazione file ricorsivo passando come parametri la gerarchia corrispondente e i file temporanei
	FCR.sh $i /tmp/files$$-$N
	#conto il numero di linee di /tmp/files$$-$N che equivale al numero di file trovati e lo salvo su /tmp/conta$$
	echo `wc -l < /tmp/files$$-$N` >> /tmp/conta$$
	#incremento contatore
	N=`expr $N + 1`
done

#ripristino contatore
N=1

#per ogni gerarchia
for i
do
	echo Nome della gerarchia $i
	echo Numero file trovati `head -$N /tmp/conta$$ | tail -1` 
	#per ogni file della gerarchia
	for j in `cat /tmp/files$$-$N`
	do
		echo Nome assoluto file $j
		echo Inserire un numero positivo
		read X
		#controllo che sia un numero positivo
		case $X in
		*[!0-9]*)	#se contiene caratteri non numerici
				echo $X non numero o non positivo
				#passo alla iterazione successiva
				continue;;
		*)		#se numero
				echo Le prime $X linee del file
				head -$X $j;;
		esac
	done
	#incremento contatore
	N=`expr $N + 1`
done

#ripristino contatore 
N=1

#rimozione file temporanei
rm /tmp/conta$$

for i
do
	rm /tmp/files$$-$N
	N=`expr $N + 1`
done
