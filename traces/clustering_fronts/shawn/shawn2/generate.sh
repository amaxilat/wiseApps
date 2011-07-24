#!/bin/bash
outfile=out.xml

echo "<?xml version=\"1.0\" ?>
<scenario>
	<snapshot id=\"0\">" > $outfile

density=$2
dimension=$1
#nodes=$1
total=0
#dimension=$density/4

echo -n "" > $outfile.tmp

for (( x=1.0; x<=$dimension; x+=0.5 ))
do
for (( y=1.0; y<=$dimension; y+=0.5 ))
do
echo "$x,$y"
for (( nodes=1; nodes<=$density; nodes++ ))
do
((total++));
echo -n "		<node id=\"v$RANDOM-QOP-C\"><location x=\"$x$RANDOM\" y=\"$y$RANDOM\" z=\"0\" /></node>
" >> $outfile.tmp
done
done
done

sort -k 2 $outfile.tmp >> $outfile

echo "$total"
echo "	</snapshot>
</scenario>" >> $outfile


mv $outfile topology_$total.xml
