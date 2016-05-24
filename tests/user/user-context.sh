#/bin/bash
printf "\nTesting user context of application defined uri:scheme \n\n"
printf "Checking for current user context ... "
URI_USER=`whoami`
printf "$URI_USER \n"
printf "Checking for uri-scheme utility   ... "
URI_APP=`./run/bin/uri-scheme --version 2>/dev/null`
if [ -z "$URI_APP" ]; then 
	printf "No\n" 
	exit 1 
else 
	printf "Yes\n" 
fi 

printf "Checking for uri scheme test://   ... "
URI_SCHEME=`./run/bin/uri-scheme -l -n test | cut -f2 2>/dev/null`
if [ -z "$URI_SCHEME" ]; then
	printf "No\n"
else
	printf "Yes (--unregister specified schema and run test again)\n"
fi

printf "\nregister uri scheme test://\n\n"
SLEEP_BIN=`cygpath.exe -w /usr/bin/sleep`
./run/bin/uri-scheme -n test --about=test --handler="$SLEEP_BIN 5" --register
printf "open uri scheme handler for test://\n"
cygstart test:// 2>/dev/null
sleep 3 2> /dev/null
printf "Checking for scheme handler pid   ... "
FIND_USER=`ps -ef | sed -e's/  */ /g' | grep sleep | grep -v grep | cut -d " " -f3`
printf "$FIND_USER\n"
printf "Checking for scheme handler user  ... "
FIND_USER=`ps -ef | sed -e's/  */ /g' | grep sleep | grep -v grep | cut -d " " -f2`
printf "$FIND_USER\n"

#sleep 5 2>/dev/null
printf "\nunregister uri scheme test://\n\n"
./run/bin/uri-scheme -n test --unregister

printf "Checking for test result          ... "

if [ "$URI_USER" == "$FIND_USER" ]; then
	printf "success\n"
else
	printf "failed\n"
fi
printf "\n\n"
