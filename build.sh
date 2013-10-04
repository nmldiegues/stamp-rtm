#!/bin/sh

FOLDERS="array bayes  genome  intruder  kmeans  labyrinth  ssca2  vacation  yada"

rm lib/*.o || true

cd ssync;
make clean;
make LOCK_VERSION="-DUSE_$2";
cd ..;

for F in $FOLDERS
do
    cd $F
    rm *.o || true
    make -f Makefile.$1 LOCK_VERSION="-DUSE_$2"
    rc=$?
    if [[ $rc != 0 ]] ; then
	echo ""
        echo "=================================== ERROR BUILDING $F $1 $2 ===================================="
	echo ""
        exit 1
    fi
    cd ..
done
