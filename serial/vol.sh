#/bin/bash
DEVICE=/dev/ttyUSB0
target=""
vol=""
line=""

function read_vol {
$(sleep 0.5 ; echo 'i' > $DEVICE) &
while ! echo $line | grep -q "Target:"; do
	line=$(head -n$1 $DEVICE)
	vol=$(echo $line | sed -e 's/.*Vol://' -e 's/ Target.*//')
	target=$(echo $line | sed -e 's/.*Target://')
	break
done

if [ -z $1 ]; then
	read_vol 1
elif [ $1 -ge 0 -a $1 -le 255 ]; then
	target=$(awk "BEGIN{printf \"%c\", $1}")
	echo "New Target: $1=$target"
	echo 't' > $DEVICE
	echo $target > $DEVICE
	read_vol 4
else
	echo "input [0-255]"
fi

echo "vol: $vol target: $target"
