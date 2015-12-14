#!/bin/bash

# echo "Argument 1: $1"
# echo "Argument 2: $2"

if [ "$1" = "--verbose" ]; then
	g++ uniprogrammed_main.cpp -o uniprogrammed
	./uniprogrammed $1 $2
	echo "  "
	echo "  "
	g++ fcfs.cpp -o fcfs
	./fcfs $1 $2
	echo "  "
	echo "  "
	g++ roundrobin_main.cpp -o roundrobin
	./roundrobin $1 $2
	echo "  "
	echo "  "
	g++ sjf.cpp -o sjf
	./sjf $1 $2
elif [ "$1" != "" ]; then
	g++ uniprogrammed_main.cpp -o uniprogrammed
	./uniprogrammed $1
	echo "  "
	echo "  "
	g++ fcfs.cpp -o fcfs
	./fcfs $1
	echo "  "
	echo "  "
	g++ roundrobin_main.cpp -o roundrobin
	./roundrobin $1
	echo "  "
	echo "  "
	g++ sjf.cpp -o sjf
	./sjf $1
else
	echo "You did not enter any arguments"
fi