#!/bin/bash

while [ 1 ]
do
	if [ -s end.txt ]
	then
	    break;
	fi
	cat /dev/input/event2 > mouse.txt&
done