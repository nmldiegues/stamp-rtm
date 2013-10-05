#!/bin/sh

FOLDERS="array bayes  genome  intruder  kmeans  labyrinth  ssca2  vacation  yada"

rm lib/*.o || true

name=$1
alias=$2

cd ssync;
make clean;
make LOCK_VERSION="-DUSE_$alias";
rc=$?
if [[ $rc != 0 ]] ; then
     echo ""
     echo "=================================== ERROR BUILDING $name $alias ==================================="
     echo ""
     exit 1
fi
cd ..;

for F in $FOLDERS
do
    cd $F
    rm *.o || true
    make -f Makefile.$name LOCK_VERSION="-DUSE_$alias"
    rc=$?
    if [[ $rc != 0 ]] ; then
	echo ""
        echo "=================================== ERROR BUILDING $F $name $alias ===================================="
	echo ""
        exit 1
    fi
    cd ..
done
