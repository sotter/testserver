#!/bin/sh

CURDIR=`cd $(dirname $0);pwd`

SERVER_NAME='tcpserver'

LOGIN_NAME=`whoami`

PROGRAM_PATH="$CURDIR"

while true
do
	for i in $SERVER_NAME
	do
		CHK=`ps -ef | grep "/$i" | grep -v "grep" | grep $LOGIN_NAME | awk -F' ' '{print $8}'`
	    PID="$CURDIR/pid/$i.pid"
	    if [ -f $PID ]; then
	        id=`cat $PID`
		    CHK=`ps -ef | grep $id | grep -v "grep" | grep "/$i" | awk -F' ' '{print $8}'`
	    fi

	    if [[ -z $CHK ]]; then
	        TEMP="$PROGRAM_PATH/$i"
	        echo $TEMP
	    	$TEMP&
	    	sleep 1
	    fi
	done
	sleep 3
done
