#!/bin/sh
export LAST=0
while :
do
	export TMP=$LAST
	export LAST=`grep page.created ./session.map | cut -f2 -d' ' | sort | tail -n 1`

#	if [[ $LAST != $TMP && $TMP != 0 ]]
	#then
export NOW=$(($(date +'%s * 1000 + %-N / 1000000')))
	export UPDATED=`expr $NOW - $LAST`
	echo "$(date +"%T.%N") object last=$LAST last-update: $UPDATED ms"
#	fi
done
