#!/bin/sh

echo "Configuring $1"

rm -rf lib
rm -rf bayes
rm -rf genome
rm -rf intruder
rm -rf kmeans
rm -rf labyrinth
rm -rf ssca2
rm -rf vacation
rm -rf yada

#if [ "$1" = "rstmnorec" ]
#then
        cp -r lib-bkup lib
        cp -r bayes-bkup bayes
        cp -r genome-bkup genome
        cp -r intruder-bkup intruder
        cp -r kmeans-bkup kmeans
        cp -r labyrinth-bkup labyrinth
        cp -r ssca2-bkup ssca2
        cp -r vacation-bkup vacation
        cp -r yada-bkup yada
#else
#        cp -r lib-ssync lib
#        cp -r bayes-ssync bayes
#        cp -r genome-ssync genome
#        cp -r intruder-ssync intruder
#        cp -r kmeans-ssync kmeans
#        cp -r labyrinth-ssync labyrinth
#        cp -r ssca2-ssync ssca2
#        cp -r vacation-ssync vacation
#        cp -r yada-ssync yada
#fi


cp lib/tm.h.$1 lib/tm.h

cp common/Defines.common.mk.$1 common/Defines.common.mk

cp common/Makefile.$1 common/Makefile.stm

cp lib/thread.h.$1 lib/thread.h

cp lib/thread.c.$1 lib/thread.c

