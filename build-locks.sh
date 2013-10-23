#!/bin/sh

FOLDERS="array-locks kmeans-locks"

rm lib/*.o || true

name=$1
alias=$2
locks=$3

if [[ $# != 1 ]] ; then
    cd ssync
    make clean
    make LOCK_VERSION="-DUSE_$alias"
    rc=$?
    if [[ $rc != 0 ]] ; then
         echo ""
         echo "=================================== ERROR BUILDING $name $alias $locks ==================================="
         echo ""
         exit 1
    fi
    cd ..;
fi

for F in $FOLDERS
do
    cd $F
    rm *.o || true
    make -f Makefile.$name LOCK_VERSION="-DUSE_$alias" LOCKS="-DLOCKS=$locks"
    rc=$?
    if [[ $rc != 0 ]] ; then
	echo ""
        echo "=================================== ERROR BUILDING $F $name $alias $locks ===================================="
	echo ""
        exit 1
    fi
    cd ..
done
