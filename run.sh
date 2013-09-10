#!/bin/sh

workspace="/home/nmld/workspace-c/stamp-rtm/"

config[1]="rtm"
config[2]="seq"
config[3]="tl2"
config[4]="tinystm"
config[5]="swisstm"

benchmarks[1]="bayes"
benchmarks[2]="genome"
benchmarks[3]="intruder"
benchmarks[4]="kmeans"
benchmarks[5]="labyrinth"
benchmarks[6]="ssca2"
benchmarks[7]="vacation"
benchmarks[8]="yada"

params[1]="-v32 -r4096 -n10 -p40 -i2 -e8 -s1"
params[2]="-g16384 -s64 -n16777216"
params[3]="-a10 -l128 -n262144 -s1"
params[4]="-m15 -n15 -t0.00001 -i inputs/random-n65536-d32-c16.txt"
params[5]="-i inputs/random-x512-y512-z7-n512.txt"
params[6]="-s20 -i1.0 -u1.0 -l3 -p3"
params[7]="-n4 -q60 -u90 -r1048576 -t4194304"
params[8]="-a15 -i inputs/ttimeu1000000.2"

ext[1]=".rtm"
ext[2]=".seq"
ext[3]=""
ext[4]=""
ext[5]=""

build[1]="rtm"
build[2]="seq"
build[3]="stm"
build[4]="stm"
build[5]="stm"


for c in 2 3 4 5
do
    cd $workspace;
    bash config.sh ${config[$c]};
    bash build.sh ${build[$c]};
    for b in 5 #6 7 8
    do
        for t in 2
        do
            for a in 1 #2 3 4 5 6 7 8
            do
                cd $workspace;
                cd ${benchmarks[$b]};
                echo "${config[$c]} | ${benchmarks[$b]} | threads $t | attempt $a"
                ./${benchmarks[$b]}${ext[$c]} ${params[$b]} > ../auto-results/${config[$c]}-${benchmarks[$b]}-$t-$a.data
                rc=$?
                if [[ $rc != 0 ]] ; then
                    echo "Error within: ${config[$c]} | ${benchmarks[$b]} | threads $t | attempt $a" >> ../auto-results/error.out
                    exit 1;
                fi
            done    
        done
    done
done
