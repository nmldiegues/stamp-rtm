#!/bin/sh

FOLDERS="array-locks"

rm lib/*.o || true

name=$1

for F in $FOLDERS
do
    cd $F
    rm *.o || true
    make -f Makefile.$name LOCKS="-DLOCKS=$2"
    rc=$?
    if [[ $rc != 0 ]] ; then
	echo ""
        echo "=================================== ERROR BUILDING $F $name $2 ===================================="
	echo ""
        exit 1
    fi
    cd ..
done
