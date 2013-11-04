#!/bin/sh

FOLDERS="array genome  intruder  kmeans  labyrinth  ssca2  vacation  yada"

rm lib/*.o || true

name=$1
alias=$2

if [[ $# != 3 ]] ; then
    prob=10
else
    prob=$3
fi

if [[ $# != 1 ]] ; then
    cd ssync
    make clean
    cp include/$alias-lock_if.h include/lock_if.h
    make LOCK_VERSION="-DUSE_$alias"
    rc=$?
    if [[ $rc != 0 ]] ; then
         echo ""
         echo "=================================== ERROR BUILDING $name $alias ==================================="
         echo ""
         exit 1
    fi
    cd ..;
fi

for F in $FOLDERS
do
    cd $F
    rm *.o || true
    make -f Makefile.$name LOCK_VERSION="-DUSE_$alias" PROB="-DFALLBACK_PROB=$prob"
    rc=$?
    if [[ $rc != 0 ]] ; then
	echo ""
        echo "=================================== ERROR BUILDING $F $name $alias ===================================="
	echo ""
        exit 1
    fi
    cd ..
done
