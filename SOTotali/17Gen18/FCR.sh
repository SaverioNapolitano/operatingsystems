#!/bin/sh
cd $1
NumFile=0
for i in *
do
	if test -f $i
	then 	if test `wc -l < $i` -eq $3
		then 	if grep $4 <  $i > /dev/null 
			then NumFile=`expr $NumFile + 1`
			fi
		fi
	fi
done

if test $NumFile -eq $2
then 	echo `pwd`
fi

for i in *
do	
	if test -d $i -a -x $i
	then	FCR.sh	`pwd`/$i $2 $3 $4
	fi
done
