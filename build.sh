#!/bin/sh

FOLDERS="bayes  genome  intruder  kmeans  labyrinth  ssca2  vacation  yada"

rm lib/*.o || true

for F in $FOLDERS
do
    cd $F
    rm *.o || true
    make -f Makefile.$1
    #cp $F.rtm ../bin
    cd ..
done
