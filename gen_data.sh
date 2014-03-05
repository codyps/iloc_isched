#! /bin/sh

S="z a b c d e"
D=./benchmarks
O=./output
B=./scheduler
C=../iloc_sim/sim

mkdir -p "$O"

for F in $D/bench??.iloc; do
	f=$(basename "$F" ".iloc")
	num=$(echo "$f" | sed 's/bench//')

	echo $F ':' $num

	printf "%s " $num >> $O/combined.csv
	printf "%s " $num >> $O/combined.tex

	for s in $S; do
		of=$O/$f.$s

		if [ $s = z ]; then
			cat $F > $of.iloc
		else
			echo "$B -$s < $F > $of.iloc"
			$B -$s < $F > $of.iloc
		fi

		$C < $of.iloc > $of.res
		cycles=$(grep Executed < $of.res | grep -v vector | awk '{ print $8 }')

		echo " cycles: $cycles"

		printf ", %s " $cycles >> $O/combined.csv
		printf "& %s " $cycles >> $O/combined.tex
	done

	echo      >> $O/combined.csv
	echo '\\' >> $O/combined.tex
done
