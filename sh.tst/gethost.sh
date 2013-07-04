
idx0=$1
idx1=$2
pref=$3
res=$4

lst=`seq $idx0 $idx1`

for h in $lst; do
    host=$pref$h.$res
    ping -c1 $host >/dev/null
    #if [ $? -ne 0 ]; then
	#echo $host
    #fi
done
