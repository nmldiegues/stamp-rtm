#!/bin/sh

FOLDERS="bayes  genome  intruder  kmeans  labyrinth  ssca2  vacation  yada"

rm lib/*.o || true

for F in $FOLDERS
do
    cd $F
    rm *.o || true
    make -f Makefile.$1
    rc=$?
    if [[ $rc != 0 ]] ; then
	echo ""
        echo "=================================== ERROR BUILDING $F $1 ===================================="
	echo ""
        exit 1
    fi
    cd ..
done
