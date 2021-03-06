#!/bin/sh

workspace="/home/ndiegues/stamp-rtm/"

cd tl2;
make clean; make;
cd ../TinySTM;
make clean; make;
cd ../swisstm;
make clean; make;
cd ../norec;
make clean; make;
cd ..

config[1]="rtmtl2"
config[2]="rtmnorec"
config[3]="tl2"
config[4]="tinystm"
config[5]="swisstm"
config[6]="norec"
config[7]="boostm"
config[8]="seq"
config[9]="seq"
config[10]="seq"
config[11]="seq"
config[12]="seq"
config[13]="seq"
config[14]="seq"
config[15]="seq"
config[16]="seq"
config[17]="rtmssync"
config[18]="rtmssync"
config[19]="rtmssync"
config[20]="rtmssync"
config[21]="rtmssync"
config[22]="rtmssync"
config[23]="rtmssync"
config[24]="rtmssync"
config[25]="rtmssync"
config[26]="rtmssyncaux"
config[27]="rtmssyncaux"
config[28]="rtmssyncaux"
config[29]="rtmssyncaux"
config[30]="rtmssyncaux"
config[31]="rtmssyncaux"
config[32]="rtmssyncaux"
config[33]="rtmssyncaux"
config[34]="rtmssyncaux"

config[50]="rtmssynconlyaux"
config[51]="rtmssyncstart"
config[52]="rtmnoaux"

config[39]="manlocks"
config[40]="manlocks"
config[41]="manlocks"
config[42]="manlocks"
config[43]="rtmmanlocks"
config[44]="rtmmanlocks"
config[45]="rtmmanlocks"

alias[1]="MCS_LOCKS"
alias[2]="MCS_LOCKS"
alias[3]="MCS_LOCKS"
alias[4]="MCS_LOCKS"
alias[5]="MCS_LOCKS"
alias[6]="MCS_LOCKS"
alias[7]="MCS_LOCKS"
alias[8]="MCS_LOCKS"
alias[9]="HCLH_LOCKS"
alias[10]="TTAS_LOCKS"
alias[11]="SPINLOCK_LOCKS"
alias[12]="ARRAY_LOCKS"
alias[13]="CLH_LOCKS"
alias[14]="RW_LOCKS"
alias[15]="TICKET_LOCKS"
alias[16]="HTICKET_LOCKS"
alias[17]="MCS_LOCKS"
alias[18]="HCLH_LOCKS"
alias[19]="TTAS_LOCKS"
alias[20]="SPINLOCK_LOCKS"
alias[21]="ARRAY_LOCKS"
alias[22]="CLH_LOCKS"
alias[23]="RW_LOCKS"
alias[24]="TICKET_LOCKS"
alias[25]="HTICKET_LOCKS"
alias[26]="MCS_LOCKS"
alias[27]="HCLH_LOCKS"
alias[28]="TTAS_LOCKS"
alias[29]="SPINLOCK_LOCKS"
alias[30]="ARRAY_LOCKS"
alias[31]="CLH_LOCKS"
alias[32]="RW_LOCKS"
alias[33]="TICKET_LOCKS"
alias[34]="HTICKET_LOCKS"

alias[39]="HTICKET_LOCKS"
alias[40]="HTICKET_LOCKS"
alias[41]="HTICKET_LOCKS"
alias[42]="HTICKET_LOCKS"
alias[43]="HTICKET_LOCKS"
alias[44]="HTICKET_LOCKS"
alias[45]="HTICKET_LOCKS"

alias[50]="HTICKET_LOCKS"
alias[51]="HTICKET_LOCKS"
alias[52]="HTICKET_LOCKS"

benchmarks[1]="redblacktree"
benchmarks[2]="genome"
benchmarks[3]="intruder"
benchmarks[4]="kmeans"
benchmarks[5]="labyrinth"
benchmarks[6]="ssca2"
benchmarks[7]="vacation"
benchmarks[8]="yada"
benchmarks[9]="array"
benchmarks[10]="array"
benchmarks[11]="array"
benchmarks[12]="array"
benchmarks[13]="array"
benchmarks[14]="array"
benchmarks[15]="array"
benchmarks[16]="array"
benchmarks[17]="array"

benchmarkslocks[2]="genome-locks"
benchmarkslocks[3]="intruder-locks"
benchmarkslocks[4]="kmeans-locks"
benchmarkslocks[6]="ssca2-locks"
benchmarkslocks[9]="array-locks"
benchmarkslocks[10]="array-locks"
benchmarkslocks[11]="array-locks"
benchmarkslocks[12]="array-locks"
benchmarkslocks[13]="array-locks"
benchmarkslocks[14]="array-locks"
benchmarkslocks[15]="array-locks"
benchmarkslocks[16]="array-locks"
benchmarkslocks[17]="array-locks"

locks[39]="10000"
locks[40]="10000"
locks[41]="1000"
locks[42]="100"
locks[43]="10000"
locks[44]="1000"
locks[45]="100"

balias[1]="redblacktree"
balias[2]="genome"
balias[3]="intruder"
balias[4]="kmeans"
balias[5]="labyrinth"
balias[6]="ssca2"
balias[7]="vacation"
balias[8]="yada"
balias[9]="array1"
balias[10]="array2"
balias[11]="array3"
balias[12]="array4"
balias[13]="array5"
balias[14]="array6"
balias[15]="array7"
balias[16]="array8"
balias[17]="array9"


params[1]="-d 30000000 -i 1024 -r 1000000 -u 10 -n "
params[2]="-g16384 -s64 -n16777216 -t"
params[3]="-a10 -l128 -n262144 -s1 -t"
params[4]="-m15 -n15 -t0.00001 -i inputs/random-n65536-d32-c16.txt -p"
params[5]="-i inputs/random-x512-y512-z7-n512.txt -t"
params[6]="-s20 -i1.0 -u1.0 -l3 -p3 -t"
params[7]="-n4 -q60 -u90 -r1048576 -t4194304 -c"
params[8]="-a15 -i inputs/ttimeu1000000.2 -t"
params[9]="-s 1000 -o10000000 -i10 -c10 -w0 -t"
params[10]="-s 1000 -o1000000 -i1000 -c10 -w0 -t"
params[11]="-s 1000 -o2000000 -i10 -c200 -w0 -t"
params[12]="-s 1000 -o10000000 -i10 -c10 -w100 -t"
params[13]="-s 1000 -o1000000 -i1000 -c10 -w100 -t"
params[14]="-s 1000 -o2000000 -i10 -c200 -w100 -t"
params[15]="-s 1000 -o10000000 -i10 -c10 -w1000 -t"
params[16]="-s 1000 -o1000000 -i1000 -c10 -w1000 -t"
params[17]="-s 1000 -o2000000 -i10 -c200 -w1000 -t"

ext[1]=".rtm"
ext[2]=".rtm"
ext[3]=""
ext[4]=""
ext[5]=""
ext[6]=""
ext[7]=""
ext[8]=".seq"
ext[9]=".seq"
ext[10]=".seq"
ext[11]=".seq"
ext[12]=".seq"
ext[13]=".seq"
ext[14]=".seq"
ext[15]=".seq"
ext[16]=".seq"
ext[17]=".rtm"
ext[18]=".rtm"
ext[19]=".rtm"
ext[20]=".rtm"
ext[21]=".rtm"
ext[22]=".rtm"
ext[23]=".rtm"
ext[24]=".rtm"
ext[25]=".rtm"
ext[26]=".rtm"
ext[27]=".rtm"
ext[28]=".rtm"
ext[29]=".rtm"
ext[30]=".rtm"
ext[31]=".rtm"
ext[32]=".rtm"
ext[33]=".rtm"
ext[34]=".rtm"

ext[50]=".rtm"
ext[51]=".rtm"
ext[52]=".rtm"

ext[39]=""
ext[40]=""
ext[41]=""
ext[42]=""
ext[43]=""
ext[44]=""
ext[45]=""


build[1]="rtm"
build[2]="rtm"
build[3]="stm"
build[4]="stm"
build[5]="stm"
build[6]="stm"
build[7]="stm"
build[8]="seq"
build[9]="seq"
build[10]="seq"
build[11]="seq"
build[12]="seq"
build[13]="seq"
build[14]="seq"
build[15]="seq"
build[16]="seq"
build[17]="rtm"
build[18]="rtm"
build[19]="rtm"
build[20]="rtm"
build[21]="rtm"
build[22]="rtm"
build[23]="rtm"
build[24]="rtm"
build[25]="rtm"
build[26]="rtm"
build[27]="rtm"
build[28]="rtm"
build[29]="rtm"
build[30]="rtm"
build[31]="rtm"
build[32]="rtm"
build[33]="rtm"
build[34]="rtm"

build[50]="rtm"
build[51]="rtm"
build[52]="rtm"

build[39]="stm"
build[40]="stm"
build[41]="stm"
build[42]="stm"
build[43]="stm"
build[44]="stm"
build[45]="stm"

wait_until_finish() {
    pid3=$1
    echo "process is $pid3"
    LIMIT=120
    for ((j = 0; j < $LIMIT; ++j)); do
        kill -s 0 $pid3
        rc=$?
        if [[ $rc != 0 ]] ; then
            echo "returning"
            return;
        fi
        sleep 1s
    done
    kill -9 $pid3
}

prob[1]=10
prob[2]=25
prob[3]=50
prob[4]=75
prob[5]=90

for p in 5
do
for c in 1 2 34 3 4 5 6 16 # 2 3 4 5 15 26 27 28 29 31 32 33 34
do
    cd $workspace;
    echo "building ${build[$c]} ${alias[$c]}"
    bash config.sh ${config[$c]};
    bash build.sh ${build[$c]} ${alias[$c]} ${prob[$p]};
    for b in 1
    do
        for t in 1 # 2 3 4 5 6 7 8
        do
            for a in 1
            do
                cd $workspace;
                cd ${benchmarks[$b]};
                echo "${config[$c]} | ${balias[$b]} | retries $r | threads $t | attempt $a | ${alias[$c]}"
                ./../../IntelPerformanceCounterMonitorV2.5.1/pcm-tsx.x 1 -c > ../auto-results/${config[$c]}-${alias[$c]}-${balias[$b]}-$t-$a.pcm &
                pid=$!
                ./../../power_gadget/power_gadget -e 100 > ../auto-results/${config[$c]}-${alias[$c]}-${balias[$b]}-$t-$a.pow &
                pid2=$!
                ./${benchmarks[$b]}${ext[$c]} ${params[$b]}$t > ../auto-results/${config[$c]}-${alias[$c]}-${balias[$b]}-$t-$a.data &
                pid3=$!
                wait_until_finish $pid3
                wait $pid3
                rc=$?
                kill -9 $pid
                kill -9 $pid2
                if [[ $rc != 0 ]] ; then
                    echo "Error within: ${alias[$c]} | ${config[$c]} | ${balias[$b]} | retries $r | threads $t | attempt $a" >> ../auto-results/error.out
                fi
            done
            cp $workspace/lib/tm.h.rtm $workspace/lib/tm.h
        done
    done
done
done

exit 0;

cd $workspace;
cp tl2/tl2.h.gv5 tl2/tl2.h;
cd tl2;
make clean;
make;
cd ..

for c in 1 # 2 3 4 5 15 26 27 28 29 31 32 33 34
do
    cd $workspace;
    echo "building ${build[$c]} ${alias[$c]}"
    bash config.sh ${config[$c]};
    bash build.sh ${build[$c]} ${alias[$c]};
    for b in 2 3 4 5 6 7 8
    do 
        for t in 1 2 3 4 5 6 7 8
        do
#        for r in 1 2 3 4 5 6
#        do
#            sed -i "s/int tries = 4/int tries = $r/g" $workspace/lib/tm.h
            for a in 6 7 8 9 10 11 12 13
            do 
                cd $workspace;
                cd ${benchmarks[$b]};
                echo "${config[$c]} gv5 | ${balias[$b]} | retries $r | threads $t | attempt $a | ${alias[$c]}"
                ./../../IntelPerformanceCounterMonitorV2.5.1/pcm-tsx.x 1 -c > ../auto-results/${config[$c]}-gv5-${alias[$c]}-${balias[$b]}-$t-$a.pcm &
                pid=$!
                ./../../power_gadget/power_gadget -e 100 > ../auto-results/${config[$c]}-gv5-${alias[$c]}-${balias[$b]}-$t-$a.pow &
                pid2=$!
                ./${benchmarks[$b]}${ext[$c]} ${params[$b]}$t > ../auto-results/${config[$c]}-gv5-${alias[$c]}-${balias[$b]}-$t-$a.data &
                pid3=$!
                wait_until_finish $pid3
                wait $pid3
                rc=$?
                kill -9 $pid
                kill -9 $pid2
                if [[ $rc != 0 ]] ; then
                    echo "Error within: ${alias[$c]}-gv5 | ${config[$c]} | ${balias[$b]} | retries $r | threads $t | attempt $a" >> ../auto-results/error.out
                fi
            done
            cp $workspace/lib/tm.h.rtm $workspace/lib/tm.h
#        done
        done
    done
done

exit 0;

for c in 3 4 5 16 34 # 2 3 4 5 15 26 27 28 29 31 32 33 34
do
    cd $workspace;
    echo "building ${build[$c]} ${alias[$c]}"
    bash config.sh ${config[$c]};
    bash build.sh ${build[$c]} ${alias[$c]};
    for b in 2 3 4 5 6 7 8
    do 
        for t in 1 2 3 4 5 6 7 8
        do
            for a in 20 21 22 23 24 25
            do 
                cd $workspace;
                cd ${benchmarks[$b]};
                echo "${config[$c]} | ${balias[$b]} | retries $r | threads $t | attempt $a | ${alias[$c]}"
                ./../../IntelPerformanceCounterMonitorV2.5.1/pcm-tsx.x 1 -c > ../auto-results/${config[$c]}-${alias[$c]}-${balias[$b]}-$t-$a.pcm &
                pid=$!
                ./../../power_gadget/power_gadget -e 100 > ../auto-results/${config[$c]}-${alias[$c]}-${balias[$b]}-$t-$a.pow &
                pid2=$!
                ./${benchmarks[$b]}${ext[$c]} ${params[$b]}$t > ../auto-results/${config[$c]}-${alias[$c]}-${balias[$b]}-$t-$a.data &
                pid3=$!
                wait_until_finish $pid3
                wait $pid3
                rc=$?
                kill -9 $pid
                kill -9 $pid2
                if [[ $rc != 0 ]] ; then
                    echo "Error within: ${alias[$c]} | ${config[$c]} | ${balias[$b]} | retries $r | threads $t | attempt $a" >> ../auto-results/error.out
                fi
            done
            cp $workspace/lib/tm.h.rtm $workspace/lib/tm.h
        done
    done
done

exit 0

for c in 26 27 28 29 30 31 32 33 34 # 2 3 4 5 15 26 27 28 29 31 32 33 34
do
    cd $workspace;
    echo "building ${build[$c]} ${alias[$c]} ${locks[$c]}"
    bash config.sh ${config[$c]};
    bash build-locks.sh ${build[$c]} ${alias[$c]} ${locks[$c]};
    for b in 2 3 4 5 6 7 8 9
    do
        for t in 1 2 3 4 5 6 7 8 12 14 16
        do
#        for r in 1 2 3 4 5 6
#        do
#            sed -i "s/int tries = 4/int tries = $r/g" $workspace/lib/tm.h
            for a in 1 2 3 4 5 #6 7 8 9 10
            do
                cd $workspace;
                cd ${benchmarkslocks[$b]};
                echo "${config[$c]} | ${balias[$b]} | retries $r | threads $t | attempt $a | ${alias[$c]}"
                ./../../IntelPerformanceCounterMonitorV2.5.1/pcm-tsx.x 1 -c > ../auto-results/${config[$c]}-${alias[$c]}-${locks[$c]}-${balias[$b]}-$t-$a.pcm &
                pid=$!
                ./../../power_gadget/power_gadget -e 100 > ../auto-results/${config[$c]}-${alias[$c]}-${locks[$c]}-${balias[$b]}-$t-$a.pow &
                pid2=$!
                ./${benchmarkslocks[$b]}${ext[$c]} ${params[$b]}$t > ../auto-results/${config[$c]}-${alias[$c]}-${locks[$c]}-${balias[$b]}-$t-$a.data &
		pid3=$!
		wait_until_finish $pid3
		wait $pid3
                rc=$?
                kill -9 $pid
                kill -9 $pid2
                if [[ $rc != 0 ]] ; then
                    echo "Error within: ${alias[$c]} | ${config[$c]} | ${balias[$b]} | retries $r | threads $t | attempt $a" >> ../auto-results/error.out
                fi
            done
            cp $workspace/lib/tm.h.rtm $workspace/lib/tm.h    
#        done
        done
    done
done


