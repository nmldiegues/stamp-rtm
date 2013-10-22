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

config[1]="rtmnorec"
config[2]="norec"
config[3]="tl2"
config[4]="tinystm"
config[5]="swisstm"
config[6]="rtmmutex"
config[7]="rtmspin"
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


benchmarks[1]="bayes"
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

benchmarkslocks[4]="kmeans-locks"
benchmarkslocks[9]="array-locks"
benchmarkslocks[10]="array-locks"
benchmarkslocks[11]="array-locks"

locks[40]="1000000"
locks[41]="1000"
locks[42]="1"
locks[43]="1000000"
locks[44]="1000"
locks[45]="1"

balias[9]="array1"
balias[10]="array2"
balias[11]="array3"

params[1]="-v32 -r4096 -n10 -p40 -i2 -e8 -s1 -t"
params[2]="-g16384 -s64 -n16777216 -t"
params[3]="-a10 -l128 -n262144 -s1 -t"
params[4]="-m15 -n15 -t0.00001 -i inputs/random-n65536-d32-c16.txt -p"
params[5]="-i inputs/random-x512-y512-z7-n512.txt -t"
params[6]="-s20 -i1.0 -u1.0 -l3 -p3 -t"
params[7]="-n4 -q60 -u90 -r1048576 -t4194304 -c"
params[8]="-a15 -i inputs/ttimeu1000000.2 -t"
params[9]="-s 1000 -o10000000 -i10 -c10 -t"
params[10]="-s 1000 -o1000000 -i1000 -c10 -t"
params[11]="-s 1000 -o2000000 -i10 -c200 -t"

ext[1]=".rtm"
ext[2]=""
ext[3]=""
ext[4]=""
ext[5]=""
ext[6]=".rtm"
ext[7]=".rtm"
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

ext[40]=""
ext[41]=""
ext[42]=""
ext[43]=""
ext[44]=""
ext[45]=""


build[1]="stm"
build[2]="stm"
build[3]="stm"
build[4]="stm"
build[5]="stm"
build[6]="rtm"
build[7]="rtm"
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

for c in 43 44 45 # 2 3 4 5 15 26 27 28 29 31 32 33 34
do
    cd $workspace;
    echo "building ${build[$c]} ${alias[$c]} ${locks[$l]}"
    bash config.sh ${config[$c]};
    bash build.sh ${build[$c]} ${alias[$c]} ${locks[$l]};
    for b in 4 9 10 11
    do
        for t in 1 2 3 4 5 6 7 8
        do
#        for r in 1 2 3 4 5 6
#        do
#            sed -i "s/int tries = 4/int tries = $r/g" $workspace/lib/tm.h
            for a in 1 ##2 3 #4 5 #6 7 8 9 10
            do
                cd $workspace;
                cd ${benchmarkslocks[$b]};
                echo "${config[$c]} | ${balias[$b]} | retries $r | threads $t | attempt $a | ${alias[$c]}"
                ./../../IntelPerformanceCounterMonitorV2.5.1/pcm-tsx.x 1 -c > ../auto-results/${config[$c]}-${alias[$c]}-${balias[$b]}-$t-$a.pcm &
                pid=$!
                ./../../power_gadget/power_gadget -e 100 > ../auto-results/${config[$c]}-${alias[$c]}-${balias[$b]}-$t-$a.pow &
                pid2=$!
                ./${benchmarkslocks[$b]}${ext[$c]} ${params[$b]}$t > ../auto-results/${config[$c]}-${alias[$c]}-${balias[$b]}-$t-$a.data &
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

