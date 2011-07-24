#echo "File is $1"
grep -e "active" -v $1 > $1.tmp
mv $1.tmp $1

awk 'BEGIN{time=1000;} \
	{
#	print $1 "aa";
#	print $2 "bb";
	if ($3=="ITERATION"){
		time=1000*$4+1000;
	}
	else{
		print time"  Text ["$0"]";
	}
	} \
	END{}' $1 > $1.conv
